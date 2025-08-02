
#pragma once

#include <string>
#include <string_view>
namespace trrt::http {
/// URL-encode (percent-encodes) string_view.
/**

    Converts bytes of original string_view representing chars out of a-zA-Z0-9.-_~ as
   %XX. The chars from that range don't get transformed.

 */
std::string urlencode(std::string_view origin);
} // namespace trrt::http