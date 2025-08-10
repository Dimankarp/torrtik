#pragma once

#include "utils/concepts.h"
#include "utils/hashing.h"
#include <iterator>
#include <serial/write.h>
#include <string>
namespace trrt::message {

namespace serial = trrt::serial;

struct HandshakeMsg {
    std::string pstr = "BitTorrent protocol";
    sha1_hash_t info_hash;
    sha1_hash_t peer_id;
};


template <std::output_iterator<char> OIt> void serialize_msg(const HandshakeMsg& msg, OIt& it) {
    serial::write_uint8(msg.pstr.size(), it);
    serial::write_range(msg.pstr, it);
    serial::write_uint8(0x00, it);
    serial::write_range(msg.info_hash, it);
    serial::write_range(msg.peer_id, it);
}


} // namespace trrt::message