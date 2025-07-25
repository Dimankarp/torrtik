#pragma once

#include "benctypes.h"
#include <istream>

namespace trrt::benc {

/*
 @brief Parses single bencoded value from stream

 @param in The istream to parse value from
 @return BencVal parsed value
 @throw std::runtime_error if fails to parse value
*/
BencVal parse_bencoding(std::istream& in);


} // namespace trrt::benc