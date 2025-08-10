#pragma once
#include "utils/hashing.h"

namespace trrt::peer {

trrt::sha1_hash_t generate_peer_id();


struct peer_status {
    bool am_choking;
    bool am_interested;
    bool peer_choking;
    bool peer_interested;
};



} // namespace trrt::peer