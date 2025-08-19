
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include "connection/tcppeer.h"
#include "manager.h"
#include "message.h"
#include "metainfo.h"
#include "msg_serialize.h"
#include "peer.h"
#include "tracker.h"
#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/beast/http/verb.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <serial/read.h>
#include <sstream>

namespace asio = boost::asio;
namespace benc = trrt::benc;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = asio::ip::tcp;


int main() {
    std::ifstream file("test.torrent");
    if(!file.is_open()) {
        perror("Failed to open file");
        exit(1);
    }
    auto tree = benc::parse_bencoding(file).get_dict();
    trrt::Metainfo meta = trrt::extract_metainfo(std::move(tree));


    asio::io_context ioc{};
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(meta.announce.hostname, "80");
    stream.connect(results);

    auto peer_id = trrt::peer::generate_peer_id();


    std::size_t cur_left =
    std::accumulate(meta.files.begin(), meta.files.end(), 0,
                    [](const auto& x, const auto& y) { return x + y.length; });

    trrt::http::TrackerRequest tracker_req{ .info_hash = meta.info_hash,
                                            .peer_id = peer_id,
                                            .port = 6881,
                                            .uploaded = 0,
                                            .downloaded = 0,
                                            .left = cur_left };


    std::string target = meta.announce.query.value_or("") + "?" +
    trrt::http::tracker_request_to_url_params(tracker_req);

    std::cout << target << "\n";

    http::request<http::string_body> req{ http::verb::get, target, 11 };
    req.set(http::field::host, meta.announce.hostname);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    std::cout << res << "\n";

    auto str_stream = std::istringstream{ res.body() };
    std::expected<trrt::http::TrackerResponse, trrt::http::TrackerFailResponse> response =
    trrt::http::extract_tracker_response(trrt::benc::parse_bencoding(str_stream).get_dict());

    if(response.has_value()) {
    } else {
        std::cout << response.error().reason << "\n";
        std::exit(1);
    }

    auto valid_response = response.value();
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);


    std::vector<char> buf;
    trrt::message::HandshakeMsg msg{ .info_hash = meta.info_hash, .peer_id = peer_id };
    auto inserter = std::back_inserter(buf);
    trrt::message::serialize_msg(msg, inserter);

    auto manager = std::make_shared<trrt::TorrentManager>(meta, peer_id);

    std::ranges::for_each(valid_response.peers, [&, mng = manager](auto& x) {
        auto conn = trrt::connection::PeerTcpConnection::create(ioc, x, manager);
        conn->start();
    });

    ioc.run();
}
