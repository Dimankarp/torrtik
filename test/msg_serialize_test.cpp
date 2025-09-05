
#include <iterator>
#define BOOST_TEST_MODULE MsgSerialize
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include "message.h"
#include "msg_serialize.h"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(ChokeMsgTest) {
    trrt::message::ChokeMsg msg{};

    std::vector<unsigned char> buf{};
    auto it = std::back_inserter(buf);
    trrt::message::serialize_msg(msg, it);

    std::vector<unsigned char> expected = { (0x00, 0x00, 0x00, 0x01,
                                             trrt::message::ChokeMsg::msg_id) };
    for(int i = 0; i < expected.size(); i++) {
        BOOST_CHECK_EQUAL(buf.at(i), expected[i]);
    }
}


BOOST_AUTO_TEST_CASE(BitfieldMsgTest) {
    trrt::message::BitfieldMsg msg{
        { true,  false, true,  false, true, false, true,  false, true,  true,
          true,  true,  true,  true,  true, true,  false, false, false, false,
          false, false, false, false, true, true,  false, true }
    };

    std::vector<unsigned char> buf{};
    auto it = std::back_inserter(buf);
    trrt::message::serialize_msg(msg, it);

    std::vector<unsigned char> expected = { (0b10101010), (0b11111111),
                                            (0b00000000), (0b11010000) };
    for(int i = 0; i < expected.size(); i++) {
        BOOST_CHECK_EQUAL(buf.at(i + 5), expected[i]);
    }
}
