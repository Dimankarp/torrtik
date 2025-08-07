
#pragma once

#include "concepts.h"
#include <iomanip>
#include <string>
namespace trrt::http {

/// URL-encode (percent-encodes) bytes from @param range.
/**

    Converts bytes of original string_view representing chars out of
   a-zA-Z0-9.-_~ as %XX. The chars from that range don't get transformed.

   @param range - range of values std::convertible_to char

 */
template <RangeOfConvertible<char> R> std::string urlencode(R range) {
    std::ostringstream out{};
    out << std::hex << std::uppercase << std::setfill('0');

    for(char c : range) {
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