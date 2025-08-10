
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include "message.h"
#include "metainfo.h"
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
#include <ostream>
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


    // target =
    // "/ann?info_hash=%EC%1F%2F%82%7D%0B%5Ep%9B%91%9F%E4w%B5%05%96%1Ev%B00&peer_id=-TK002-1410301630145&port=6881&uploaded=0&downloaded=0&left=1563609088&compact=1&event=started";

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


    tcp::socket socket{ (ioc) };
    valid_response.peers.emplace_back(boost::asio::ip::make_address_v4(
                                      "188.126.44.173"),
                                      57420);

    for(;;) {


        std::cout << "Connecting to" << valid_response.peers.back() << std::endl;
        try {

            tcp::socket temp_socket(ioc);
            struct timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            // This requires accessing the native socket
            temp_socket.open(tcp::v4());
            ::setsockopt(temp_socket.native_handle(), SOL_SOCKET, SO_SNDTIMEO,
                         &timeout, sizeof(timeout));

            temp_socket.connect(valid_response.peers.back());
            socket = std::move(temp_socket);
            break;
        } catch(...) {
            socket.close();
            valid_response.peers.pop_back();
        }
    }
    std::cout << "Sending to" << valid_response.peers.back();
    socket.send(asio::buffer(buf));
    std::cout << "Receiving from" << valid_response.peers.back();
    std::vector<char> recv_buf(1024);
    int received = 0;

    socket.async_receive(asio::buffer(recv_buf), [&](auto e, auto sz) {
        received += sz;
        std::cout << e << "\n";
        std::cout << "Received " << received << "\n";
        std::cout << std::string{ recv_buf.data() };
        char* ptr = recv_buf.data();
        std::size_t ptrsz = trrt::serial::read_uint8(ptr);
        if(ptrsz + 8 + trrt::message::HandshakeMsg::FIXED_SZ >= received) {
            std::cout << "Received all \n";
            socket.close();
        } else {
            std::cout << "didnt receive all\n";
            socket.close();
        }
    });

    ioc.run();


    // std::istringstream strstream{ res.body() };
    // auto valid_peers = trrt::benc::parse_bencoding(strstream).get_dict();
    // auto ips = valid_peers.at("peers")->get_str().val;
    // // std::cout << urlencode(ips) << "\n";
    // std::vector<std::pair<std::string, std::string>> parsed_peers{};

    // auto* start = ips.data();
    // for(int i = 0; i < ips.size() / 6; i++) {
    //     std::string ip = std::to_string(read_uint8(start));
    //     for(int j = 0; j < 3; j++) {
    //         ip += "." + std::to_string(read_uint8(start));
    //     }
    //     std::string port = std::to_string(read_uint16(start));
    //     parsed_peers.emplace_back(std::move(ip), std::move(port));
    // }
    // std::ranges::for_each(parsed_peers, [](const auto& x) {
    //     std::cout << x.first << " " << x.second << "\n";
    // });


    // tcp::resolver tcp_resolver(ioc);
    // auto& peer_one = parsed_peers.front();
    // auto peer_ends = tcp_resolver.resolve(peer_one.first, peer_one.second);

    // tcp::socket socket(ioc);
    // boost::asio::connect(socket, peer_ends);


    // // tcp_resolver.resolve(boost::asio::ip::make_address_v4(""));


    // if(ec && ec != beast::errc::not_connected)
    //     throw beast::system_error{ ec };
}
