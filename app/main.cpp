
#include "bencoding/bdec.h"
#include "bencoding/benc.h"
#include "bencoding/benctypes.h"
#include "trrtconfig.h"
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/beast/http/verb.hpp>
#include <cctype>
#include <concepts>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <random>
#include <sstream>

namespace asio = boost::asio;
namespace benc = trrt::benc;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = asio::ip::tcp;


std::string bytes_to_hex(const std::string& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for(unsigned char byte : bytes) {
        ss << std::setw(2) << static_cast<unsigned int>(byte);
    }
    return ss.str();
}


std::string generate_peer_id() {
    const int MAGIC_PRIME_1 = 7229;
    const int MAGIC_PRIME_2 = 1759;
    const int PEER_ID_BYTES = 20;
    const std::string client_prefix = "TK";
    const std::string version_str = std::to_string(PROJECT_VERSION_MAJOR) +
    std::to_string(PROJECT_VERSION_MINOR) + std::to_string(PROJECT_VERSION_PATCH);

    std::random_device rd{};
    std::linear_congruential_engine<uint16_t, MAGIC_PRIME_1, MAGIC_PRIME_2, 0> lce{
        static_cast<uint16_t>(rd())
    };
    std::string result{};

    result.reserve(PEER_ID_BYTES);
    result += "-" + client_prefix + version_str + "-";
    while(result.length() != PEER_ID_BYTES) {
        result += std::to_string(lce() % 10);
    }
    return result;
}

int main() {
    asio::io_context io{};
    std::ifstream file("test.torrent");
    if(!file.is_open()) {
        perror("Failed to open file");
        exit(1);
    }
    auto tree = benc::parse_bencoding(file).get_dict();

    auto str = trrt::benc::bencode(tree.at("info")->get_dict());
    unsigned char hash[20];
    unsigned int hash_len = 0;
    // str.val = bytes_to_hex(str.val);
    trrt::benc::print_human_readable(tree, std::cout);

    const auto filelen =
    std::to_string(tree.at("info")->get_dict().at("length")->get_int().val);


    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr);
    EVP_DigestUpdate(ctx, str.c_str(), str.length());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    std::string hash_str(reinterpret_cast<char*>(hash), 0, hash_len);
    // std::cout << urlencode(hash_str) << "\n
    auto const peer_id = generate_peer_id();
    std::cout << peer_id << "\n";

    auto const host = "bt3.t-ru.org"; // tree.at("announce")->get_str().val;
    std::cout << "Host " << host << "\n";
    asio::io_context ioc{};
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(host, "80");

    // stream.connect(results);
    // std::vector<std::pair<std::string, std::string>> params = {
    //     { "info_hash", urlencode(hash_str) },
    //     { "peer_id", peer_id },
    //     { "port", "6881" },
    //     { "uploaded", "0" },
    //     { "downloaded", "0" },
    //     { "left", filelen },
    //     { "compact", "1" },
    //     { "event", "started" }
    // };
    // std::string target = "/ann?";
    // for(int i = 0; i < params.size(); i++) {
    //     auto& [key, val] = params[i];
    //     target += key + "=" + val;
    //     if(i != params.size() - 1) {
    //         target += "&";
    //     }
    // }

    // // target =
    // // "/ann?info_hash=%ec%1f%2f%82%7d%0b%5ep%9b%91%9f%e4w%b5%05%96%1ev%b00&peer_"
    // // "id=-qB4650-QVo*oP.P4Z(*&port=35757&uploaded=0&downloaded=0&left="
    // // "1563609088&corrupt=0&key=7E82539F&event=started&numwant=200&compact=1&no_"
    // // "peer_id=1&supportcrypto=1&redundant=0";

    // http::request<http::string_body> req{ http::verb::get, target, 11 };
    // req.set(http::field::host, host);
    // req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // http::write(stream, req);

    // beast::flat_buffer buffer;
    // http::response<http::string_body> res;
    // http::read(stream, buffer, res);
    // // std::cout << res << '\n';

    // beast::error_code ec;
    // stream.socket().shutdown(tcp::socket::shutdown_both, ec);


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
