#pragma once

#include "manager.h"
#include "message.h"
#include "serial/read.h"
#include "utils/hashing.h"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <memory>
#include <utility>

namespace trrt::connection {

using tcp = boost::asio::ip::tcp;
namespace asio = boost::asio;


namespace {
template <typename Msg>
void serialize_into_streambuf(asio::streambuf& buf, const Msg& msg) {
    auto mut_buf = buf.prepare(256);
    auto it = asio::buffers_begin(mut_buf);
    auto begin = it;
    message::serialize_msg(msg, it);
    buf.commit(std::distance(begin, it));
}
} // namespace


class PeerTcpConnection : public std::enable_shared_from_this<PeerTcpConnection> {


    private:
    using pointer = std::shared_ptr<PeerTcpConnection>;
    using manager_ptr = std::shared_ptr<TorrentManager>;
    const static std::size_t MAX_BUF_SZ = (1 << 14) + 256; // 16KB max block + 256 for bookkeeping
    constexpr const static boost::posix_time::time_duration TIMEOUT_SECS{ 0, 0, 15, 0 };

    tcp::socket socket;
    tcp::endpoint endpoint;
    manager_ptr manager;
    asio::streambuf buf;

    boost::asio::deadline_timer timer;
    bool async_op_completed = false;

    private:
    PeerTcpConnection(asio::io_context& ctx, tcp::endpoint endp, manager_ptr mngr)
    : socket{ ctx }, endpoint{ std::move(endp) }, manager{ std::move(mngr) },
      buf{ MAX_BUF_SZ }, timer{ ctx } {}

    public:
    static pointer create(asio::io_context& ctx, tcp::endpoint endp, manager_ptr mngr) {
        return pointer{ new PeerTcpConnection{ ctx, std::move(endp), std::move(mngr) } };
    }

    template <typename AsyncFunc, typename Handler>
    void run_with_timeout(AsyncFunc&& async_func, Handler&& handler) {


        timer.cancel();
        async_op_completed = false;

        async_func([self = shared_from_this(),
                    handler = std::forward<Handler>(handler)](const auto& ec, auto&&... args) mutable {
            self->async_op_completed = true;
            self->timer.cancel();
            handler(ec, std::forward<decltype(args)>(args)...);
        });

        timer.expires_from_now(TIMEOUT_SECS);
        timer.async_wait([self = shared_from_this()](const auto& ec) {
            if(ec)
                return;
            std::cout << "Deadline timer for " << self->endpoint << "\n";
            self->socket.cancel();
        });
    }

    void start() {

        auto handler = [self = shared_from_this()](const auto& ec) {
            if(ec) {
                std::cout << "Failed to connect to:" << self->endpoint << " "
                          << ec.message() << "\n";
                return;
            }
            self->send_handshake();
        };

        run_with_timeout(
        [this](auto&& h) {
            socket.async_connect(endpoint, std::forward<decltype(h)>(h));
        },
        handler);
    }

    private:
    void send_handshake() {
        // TODO(dimankarp):
        message::HandshakeMsg msg{ .info_hash = manager->info_hash(),
                                   .peer_id = manager->peer_id() };
        serialize_into_streambuf(buf, msg);

        auto handler = [self = shared_from_this()](const auto& ec, auto sz) {
            if(ec) {
                std::cout << "Failed to write handshake to:" << self->endpoint
                          << " " << ec.message() << "\n";
                return;
            }
            self->receive_handshake_stage1();
        };

        run_with_timeout(
        [this](auto&& h) {
            asio::async_write(socket, buf, std::forward<decltype(h)>(h));
        },
        handler);
    }

    void receive_handshake_stage1() {

        auto handler = [self = shared_from_this()](const auto& ec, auto /*sz*/) {
            if(ec) {
                std::cout << "Failed receive handshake byte to "
                          << self->endpoint << " " << ec.message() << "\n";
                return;
            }
            auto it = asio::buffers_begin(self->buf.data());
            auto pstrlen = serial::read_uint8(it);
            self->buf.consume(sizeof(std::uint8_t));
            std::cout << "Received pstrelen" << std::to_string(pstrlen)
                      << "from " << self->endpoint << "\n";
            self->receive_handshake_stage2(pstrlen);
        };

        run_with_timeout(
        [this](auto&& h) {
            asio::async_read(socket, buf, boost::asio::transfer_at_least(1),
                             std::forward<decltype(h)>(h));
        },
        handler);
    };

    void receive_handshake_stage2(std::uint8_t pstrlen) {

        auto handler = [&, self = shared_from_this()](const auto& ec, auto /*sz*/) {
            // Skipping protocol
            self->buf.consume(pstrlen);
            // Skipping reserved bytes
            self->buf.consume(sizeof(std::uint64_t));
            sha1_hash_t info_hash;
            sha1_hash_t peer_id;
            auto it = asio::buffers_begin(self->buf.data());
            auto start = it;
            serial::read_range(info_hash, it);
            serial::read_range(peer_id, it);
            buf.consume(std::distance(start, it));

            std::ranges::for_each(info_hash,
                                  [](auto x) { std::cout << std::hex << x; });
            std::cout << "\n";
            std::ranges::for_each(peer_id,
                                  [](auto x) { std::cout << std::hex << x; });
            std::cout << "\n";
        };

        run_with_timeout(
        [this, pstrlen](auto&& h) {
            asio::async_read(socket, buf,
                             boost::asio::transfer_at_least(pstrlen + message::HandshakeMsg::FIXED_SZ),
                             std::forward<decltype(h)>(h));
        },
        handler);
    };
};
} // namespace trrt::connection
