#pragma once

#include <cstdint>
#include <iterator>
#include <message.h>
#include <serial/write.h>
#include <string>
namespace trrt::message {

namespace serial = trrt::serial;


template <std::output_iterator<char> OIt>
void serialize_msg(const HandshakeMsg& msg, OIt& it) {
    serial::write_uint8(msg.pstr.size(), it);
    serial::write_range(msg.pstr, it);
    serial::write_uint64(0x00, it);
    serial::write_range(msg.info_hash, it);
    serial::write_range(msg.peer_id, it);
}


template <std::output_iterator<char> OIt>
void serialize_msg(const KeepAliveMsg& /*msg*/, OIt& it) {
    serial::write_uint8(0, it);
}


template <std::output_iterator<char> OIt>
void serialize_msg(const ChokeMsg& /*msg*/, OIt& it) {
    serial::write_uint8(1, it);
    serial::write_uint8(MessageId::CHOKE, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const UnchokeMsg& /*msg*/, OIt& it) {
    serial::write_uint8(1, it);
    serial::write_uint8(MessageId::UNCHOKE, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const InterestedMsg& /*msg*/, OIt& it) {
    serial::write_uint8(1, it);
    serial::write_uint8(MessageId::INTERESTED, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const NotInterestedMsg& /*msg*/, OIt& it) {
    serial::write_uint8(1, it);
    serial::write_uint8(MessageId::NOT_INTERESTED, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const HaveMsg& msg, OIt& it) {
    serial::write_uint8(1 + sizeof(msg.piece_index), it);
    serial::write_uint8(MessageId::HAVE, it);
    serial::write_uint32(msg.piece_index, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const BitfieldMsg& msg, OIt& it) {
    static const int BITS_PER_BYTE = 8;
    static const std::size_t ceiled_bytes =
    ((msg.bitfield.size() + BITS_PER_BYTE - 1) / BITS_PER_BYTE);
    serial::write_uint8(1 + ceiled_bytes, it);
    serial::write_uint8(MessageId::BITFIELD, it);

    auto bit_it = msg.bitfield.begin();

    // Full bytes
    std::size_t full_bytes = msg.bitfield.size() / BITS_PER_BYTE;
    for(int byte = 0; byte < full_bytes; byte++) {
        std::uint8_t b = *bit_it++;
        for(int i = 0; i < BITS_PER_BYTE - 1; i++) {
            b <<= 1;
            b |= *bit_it++;
        }
        serial::write_uint8(b, it);
    }

    // Left bits
    auto left = msg.bitfield.size() % BITS_PER_BYTE;
    std::uint8_t b = *bit_it++;
    for(int i = 0; i < left - 1; i++) {
        b <<= 1;
        b |= *bit_it++;
    }
    b <<= (BITS_PER_BYTE - left);
    serial::write_uint8(b, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const RequestMsg& msg, OIt& it) {
    serial::write_uint8(1 + (3 * sizeof(std::uint32_t)), it);
    serial::write_uint8(MessageId::REQUEST, it);
    serial::write_uint32(msg.index, it);
    serial::write_uint32(msg.begin, it);
    serial::write_uint32(msg.length, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const PieceMsg& msg, OIt& it) {
    serial::write_uint8(1 + (2 * sizeof(std::uint32_t) + msg.block.size()), it);
    serial::write_uint8(MessageId::PIECE, it);
    serial::write_uint32(msg.index, it);
    serial::write_uint32(msg.begin, it);
    serial::write_range(msg.block, it);
}

template <std::output_iterator<char> OIt>
void serialize_msg(const CancelMsg& msg, OIt& it) {
    serial::write_uint8(1 + (3 * sizeof(std::uint32_t)), it);
    serial::write_uint8(MessageId::CANCEL, it);
    serial::write_uint32(msg.index, it);
    serial::write_uint32(msg.begin, it);
    serial::write_range(msg.length, it);
}


} // namespace trrt::message