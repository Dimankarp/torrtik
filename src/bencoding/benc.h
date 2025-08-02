#pragma once

#include "benctypes.h"

namespace trrt::benc {

/// Encodes @ref val with bencoding
/**
 @param val any BencVal object
 @return string containing encoded value
*/
std::string bencode(const BencVal& val);


} // namespace trrt::benc