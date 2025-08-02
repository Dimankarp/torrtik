
#include "bencoding/benc.h"
#define BOOST_TEST_MODULE BenEncoding
#include "bencoding/benctypes.h"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(Int) {
    trrt::benc::BencInt val = 15;
    BOOST_CHECK_EQUAL(trrt::benc::bencode(val), "i15e");
}

BOOST_AUTO_TEST_CASE(Int_negative) {
    trrt::benc::BencInt val = -1;
    BOOST_CHECK_EQUAL(trrt::benc::bencode(val), "i-1e");
}

BOOST_AUTO_TEST_CASE(Str) {
    trrt::benc::BencString val = "vasya";
    BOOST_CHECK_EQUAL(trrt::benc::bencode(val), "5:vasya");
}

BOOST_AUTO_TEST_CASE(Str_Empty) {
    trrt::benc::BencString val = "";
    BOOST_CHECK_EQUAL(trrt::benc::bencode(val), "0:");
}

BOOST_AUTO_TEST_CASE(Heterogenic_List) {
    trrt::benc::BencList val = { { trrt::benc::BencInt{ 15 },
                                   trrt::benc::BencString{ "test" } } };
    BOOST_CHECK_EQUAL(trrt::benc::bencode(val), "li15e4:teste");
}

BOOST_AUTO_TEST_CASE(Heterogenic_Dict) {
    trrt::benc::BencDict val{};
    val.val["a"] = trrt::make_deep<trrt::benc::BencVal>(15);
    val.val["bc"] = trrt::make_deep<trrt::benc::BencVal>(trrt::benc::BencList{
    { trrt::benc::BencInt{ 15 }, trrt::benc::BencString{ "test" } } });
    BOOST_CHECK_EQUAL(trrt::benc::bencode(val), "d1:ai15e2:bcli15e4:testee");
}
