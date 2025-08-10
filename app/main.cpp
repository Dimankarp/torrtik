
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
#include <fstream>
#include <iostream>
#include <numeric>
#include <openssl/evp.h>
#include <openssl/types.h>
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


    // // target =
    // // "/ann?info_hash=%ec%1f%2f%82%7d%0b%5ep%9b%91%9f%e4w%b5%05%96%1ev%b00&peer_"
    // // "id=-qB4650-QVo*oP.P4Z(*&port=35757&uploaded=0&downloaded=0&left="
    // // "1563609088&corrupt=0&key=7E82539F&event=started&numwant=200&compact=1&no_"
    // // "peer_id=1&supportcrypto=1&redundant=0";

    http::request<http::string_body> req{ http::verb::get, target, 11 };
    req.set(http::field::host, meta.announce.hostname);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    auto str_stream = std::istringstream{ res.body() };
    std::expected<trrt::http::TrackerResponse, trrt::http::TrackerFailResponse> response =
    trrt::http::extract_tracker_response(trrt::benc::parse_bencoding(str_stream).get_dict());

    if(response.has_value()) {
        auto& value = response.value();
        value.tracker_id.and_then([](auto& x) {
            std::cout << x << "\n";
            return std::optional<int>{};
        });

    } else {
        std::cout << response.error().reason << "\n";
    }

    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);


    std::string buf;
    buf.resize(1024);


    trrt::message::HandshakeMsg msg{ .info_hash = meta.info_hash, .peer_id = peer_id };
    char* ptr = buf.data();

    trrt::message::serialize_msg(msg, ptr);

    std::cout << buf << "\n";


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
