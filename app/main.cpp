
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include <boost/asio.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace asio = boost::asio;
namespace benc = trrt::benc;

std::string bytes_to_hex(const std::string& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for(unsigned char byte : bytes) {
        ss << std::setw(2) << static_cast<unsigned int>(byte);
    }
    return ss.str();
}

int main() {


    asio::io_context io{};
    std::ifstream file("test.torrent");
    if(!file.is_open()) {
        perror("Failed to open file");
        exit(1);
    }
    auto tree = benc::parse_bencoding(file).get_dict();

    auto& str = tree.at("info")->get_dict().at("pieces")->get_str();

    // str.val = bytes_to_hex(str.val);
    trrt::benc::print_human_readable(tree, std::cout);
}
