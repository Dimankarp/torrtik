
#include "msg_deserialize.h"
#include "serial/write.h"
#include <boost/test/tools/old/interface.hpp>
#include <iterator>
#include <variant>
#define BOOST_TEST_MODULE MsgDeserialize
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include "message.h"
#include "msg_serialize.h"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(ChokeMsgDeserialize) {
    std::vector<uint8_t> buf{};
    auto it = std::back_inserter(buf);
    trrt::serial::write_uint8(trrt::message::ChokeMsg::msg_id, it);
    using msgs = std::variant<trrt::message::RequestMsg, trrt::message::ChokeMsg>;
    auto buf_it = buf.begin();
    auto msg =
    trrt::message::deserialize<decltype(buf)::iterator, trrt::message::RequestMsg,
                               trrt::message::ChokeMsg>(buf_it, buf.end());
    BOOST_CHECK(std::holds_alternative<trrt::message::ChokeMsg>(msg));
}


BOOST_AUTO_TEST_CASE(BitfieldDeserialize) {
    std::vector<uint8_t> buf{};
    auto it = std::back_inserter(buf);
    trrt::serial::write_uint8(trrt::message::BitfieldMsg::msg_id, it);
    trrt::serial::write_uint8(0xff, it);
    trrt::serial::write_uint8(0x80, it);

    using msgs =
    std::variant<trrt::message::RequestMsg, trrt::message::ChokeMsg, trrt::message::BitfieldMsg>;
    auto buf_it = buf.begin();
    auto msg =
    trrt::message::deserialize<decltype(buf)::iterator, trrt::message::RequestMsg, trrt::message::ChokeMsg,
                               trrt::message::BitfieldMsg>(buf_it, buf.end());


    trrt::message::BitfieldMsg expected{ { true, true, true, true, true, true,
                                           true, true, true, false, false,
                                           false, false, false, false, false } };
    auto bitfield = std::get<trrt::message::BitfieldMsg>(msg).bitfield;
    BOOST_CHECK_EQUAL_COLLECTIONS(bitfield.begin(), bitfield.end(),
                                  expected.bitfield.begin(), expected.bitfield.end());
}
