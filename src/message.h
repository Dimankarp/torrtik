#pragma once

#include "serial/read.h"
#include "utils/hashing.h"
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <serial/write.h>
#include <stdexcept>
#include <string>
#include <vector>
namespace trrt::message {


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
};

struct KeepAliveMsg {
    // empty
};


template <std::input_iterator InIt>
void expect_at_least(const InIt it, const InIt end, const std::size_t expected, const char* error_msg) {
    if(std::distance(it, end) < expected)
        throw std::runtime_error{ error_msg };
}

struct ChokeMsg {
    static const MessageId msg_id = CHOKE;

    template <std::input_iterator InIt>
    static ChokeMsg deserialize_msg(InIt& /*it*/, const InIt /*end*/) {
        return ChokeMsg{};
    }
};

struct UnchokeMsg {
    static const MessageId msg_id = UNCHOKE;
    template <std::input_iterator InIt>
    static UnchokeMsg deserialize_msg(InIt& /*it*/, const InIt /*end*/) {
        return UnchokeMsg{};
    }
};

struct InterestedMsg {
    static const MessageId msg_id = INTERESTED;
    template <std::input_iterator InIt>
    static InterestedMsg deserialize_msg(InIt& /*it*/, const InIt /*end*/) {
        return InterestedMsg{};
    }
};

struct NotInterestedMsg {
    static const MessageId msg_id = NOT_INTERESTED;
    template <std::input_iterator InIt>
    static NotInterestedMsg deserialize_msg(InIt& /*it*/, const InIt /*end*/) {
        return NotInterestedMsg{};
    }
};

struct HaveMsg {
    std::uint32_t piece_index;
    static const MessageId msg_id = HAVE;

    template <std::input_iterator InIt>
    static HaveMsg deserialize_msg(InIt& it, const InIt end) {
        expect_at_least(it, end, sizeof(std::uint32_t),
                        "Not enough bytes to serialize Have message");
        std::uint32_t piece_index = serial::read_uint32(it);
        return HaveMsg{ piece_index };
    }
};

struct BitfieldMsg {
    std::vector<bool> bitfield;
    static const MessageId msg_id = BITFIELD;

    template <std::input_iterator InIt>
    static BitfieldMsg deserialize_msg(InIt& it, const InIt end) {
        const int BITS_PER_BYTE = 8;
        std::vector<bool> bitfield(std::distance(it, end) * BITS_PER_BYTE, false);
        auto bit_it = bitfield.begin();
        while(it != end) {
            std::uint8_t byte = serial::read_uint8(it);
            std::uint8_t mask = 0x80;
            for(int i = 0; i < BITS_PER_BYTE; i++) {
                *bit_it = (byte & mask) != 0;
                bit_it++;
                mask >>= 1;
            }
        }
        return BitfieldMsg{ std::move(bitfield) };
    }
};

struct RequestMsg {
    std::uint32_t index;
    std::uint32_t begin;
    std::uint32_t length;
    static const MessageId msg_id = REQUEST;


    template <std::input_iterator InIt>
    static RequestMsg deserialize_msg(InIt& it, const InIt end) {
        expect_at_least(it, end, sizeof(std::uint32_t) * 3,
                        "Not enough bytes to serialize Request message");
        std::uint32_t index = serial::read_uint32(it);
        std::uint32_t begin = serial::read_uint32(it);
        std::uint32_t length = serial::read_uint32(it);
        return RequestMsg{ .index = index, .begin = begin, .length = length };
    }
};

struct PieceMsg {
    std::uint32_t index;
    std::uint32_t begin;
    std::span<char> block;
    static const MessageId msg_id = PIECE;

    template <std::input_iterator InIt>
    static PieceMsg deserialize_msg(InIt& it, const InIt end) {
        expect_at_least(it, end, sizeof(std::uint32_t) * 2,
                        "Not enough bytes to serialize Piece message");
        std::uint32_t index = serial::read_uint32(it);
        std::uint32_t begin = serial::read_uint32(it);

        return PieceMsg{ .index = index, .begin = begin, .block = std::span<char>(it, end) };
    }
};

struct CancelMsg {
    std::uint32_t index;
    std::uint32_t begin;
    std::uint32_t length;
    static const MessageId msg_id = CANCEL;

    template <std::input_iterator InIt>
    static CancelMsg deserialize_msg(InIt& it, const InIt end) {
        expect_at_least(it, end, sizeof(std::uint32_t) * 2,
                        "Not enough bytes to serialize Cancel message");
        std::uint32_t index = serial::read_uint32(it);
        std::uint32_t begin = serial::read_uint32(it);
        std::uint32_t length = serial::read_uint32(it);
        return CancelMsg{ .index = index, .begin = begin, .length = length };
    }
};


} // namespace trrt::message