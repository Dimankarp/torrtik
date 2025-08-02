#include "urlencode.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace trrt::http {

std::string urlencode(std::string_view origin) {
    std::ostringstream out{};
    out << std::hex << std::uppercase << std::setfill('0');

    for(char c : origin) {
        if((std::isalnum(c) != 0) || c == '.' || c == '-' || c == '_' || c == '~') {
            out << c;
        } else {
            out << '%' << std::setw(2)
                << static_cast<unsigned int>(static_cast<unsigned char>(c));
        }
    }
    return out.str();
}
} // namespace trrt::http