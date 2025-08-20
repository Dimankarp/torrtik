#pragma once

#include "utils/concepts.h"
#include "utils/hashing.h"
#include <cstddef>
#include <iterator>
#include <memory>
#include <serial/write.h>
#include <string>
#include <vector>
namespace trrt::message {

namespace serial = trrt::serial;

struct HandshakeMsg {

    std::string pstr = "BitTorrent protocol";
    sha1_hash_t info_hash;
    sha1_hash_t peer_id;

    const static std::size_t FIXED_SZ =
    sizeof(info_hash) + sizeof(peer_id) + sizeof(std::uint64_t);
};

enum MessageId : char {
    CHOKE = 0,
    UNCHOKE = 1,
    INTERESTED = 2,
    NOT_INTERESTED = 3,
    HAVE = 4,
    BITFIELD = 5,
    REQUEST = 6,
    PIECE = 7,
    CANCEL = 8,
    INVALID_ID = 127
};

struct KeepAliveMsg {
    // empty
    static const MessageId msg_id = INVALID_ID;
};


struct ChokeMsg {
    static const MessageId msg_id = CHOKE;
};

struct UnchokeMsg {
    static const MessageId msg_id = UNCHOKE;
};

struct InterestedMsg {
    static const MessageId msg_id = INTERESTED;
};

struct NotInterestedMsg {
    static const MessageId msg_id = NOT_INTERESTED;
};

struct HaveMsg {
    std::uint32_t piece_index;
    static const MessageId msg_id = HAVE;
};

struct BitfieldMsg {
    std::vector<bool> bitfield;
    static const MessageId msg_id = BITFIELD;
};

struct RequestMsg {
    std::uint32_t index;
    std::uint32_t begin;
    std::uint32_t length;
    static const MessageId msg_id = REQUEST;
};

struct PieceMsg {
    std::uint32_t index;
    std::uint32_t begin;
    std::span<char> block;
    static const MessageId msg_id = PIECE;
};

struct CancelMsg {
    std::uint32_t index;
    std::uint32_t begin;
    std::uint32_t length;
    static const MessageId msg_id = CANCEL;
};


} // namespace trrt::message