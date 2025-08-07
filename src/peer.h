#pragma once
#include "utils/hashing.h"

namespace trrt::peer {

trrt::sha1_hash_t generate_peer_id();
} // namespace trrt::peer