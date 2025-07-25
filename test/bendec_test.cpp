
#define BOOST_TEST_MODULE BenDecoding
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include <boost/test/included/unit_test.hpp>


// *** INT ***

BOOST_AUTO_TEST_CASE(Regular_Int) {
    std::stringstream str{ "i53e" };
    auto val = trrt::benc::parse_bencoding(str);
    BOOST_CHECK_EQUAL(std::get<trrt::benc::BencInt>(val).val, 53);
}

BOOST_AUTO_TEST_CASE(Zero_Int) {
    std::stringstream str{ "i0e" };
    auto val = trrt::benc::parse_bencoding(str);
    BOOST_CHECK_EQUAL(std::get<trrt::benc::BencInt>(val).val, 0);
}

BOOST_AUTO_TEST_CASE(Negative_Int) {
    std::stringstream str{ "i-100e" };
    auto val = trrt::benc::parse_bencoding(str);
    BOOST_CHECK_EQUAL(std::get<trrt::benc::BencInt>(val).val, -100);
}

BOOST_AUTO_TEST_CASE(No_Int_Throw) {
    std::stringstream str{ "ie" };
    BOOST_CHECK_THROW(trrt::benc::parse_bencoding(str), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Invalid_Int_Throw) {
    std::stringstream str{ "iadsade" };
    BOOST_CHECK_THROW(trrt::benc::parse_bencoding(str), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Invalid_Int_Format_Throw) {
    std::stringstream str{ "i2324b" };
    BOOST_CHECK_THROW(trrt::benc::parse_bencoding(str), std::runtime_error);
}

// *** STRING ***

BOOST_AUTO_TEST_CASE(Regular_String) {
    std::stringstream str{ "3:abc" };
    auto val = trrt::benc::parse_bencoding(str);
    BOOST_CHECK_EQUAL(std::get<trrt::benc::BencString>(val).val, "abc");
}

BOOST_AUTO_TEST_CASE(Zero_String) {
    std::stringstream str{ "0:" };
    auto val = trrt::benc::parse_bencoding(str);
    BOOST_CHECK_EQUAL(std::get<trrt::benc::BencString>(val).val, "");
}

BOOST_AUTO_TEST_CASE(Invalid_Fromat_string) {
    std::stringstream str{ "4abc" };
    BOOST_CHECK_THROW(trrt::benc::parse_bencoding(str), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Too_Big_Length_String) {
    std::stringstream str{ "10:abc" };
    BOOST_CHECK_THROW(trrt::benc::parse_bencoding(str), std::runtime_error);
}

// *** LIST ***

BOOST_AUTO_TEST_CASE(Homogenic_List) {
    std::stringstream str{ "li25ei65ee" };
    auto val = trrt::benc::parse_bencoding(str);
    trrt::benc::BencList real = { { trrt::benc::BencInt{ 25 }, trrt::benc::BencInt{ 65 } } };
    BOOST_CHECK(std::get<trrt::benc::BencList>(val) == real);
}

BOOST_AUTO_TEST_CASE(Heterogenic_List) {
    std::stringstream str{ "li25e5:12345e" };
    auto val = trrt::benc::parse_bencoding(str);
    trrt::benc::BencList real = { { trrt::benc::BencInt{ 25 },
                                    trrt::benc::BencString{ "12345" } } };
    BOOST_CHECK(std::get<trrt::benc::BencList>(val) == real);
}

BOOST_AUTO_TEST_CASE(Empty_List) {
    std::stringstream str{ "le" };
    auto val = trrt::benc::parse_bencoding(str);
    trrt::benc::BencList real{};
    BOOST_CHECK(std::get<trrt::benc::BencList>(val) == real);
}

// *** DICT ***

BOOST_AUTO_TEST_CASE(Homogenic_Dict) {
    std::stringstream str{ "d2:abi25e3:bcdi26ee" };
    auto val = trrt::benc::parse_bencoding(str);
    trrt::benc::BencDict real{};
    real.val["ab"] = trrt::make_deep<trrt::benc::BencVal>(25);
    real.val["bcd"] = trrt::make_deep<trrt::benc::BencVal>(26);
    BOOST_CHECK(std::get<trrt::benc::BencDict>(val) == real);
};


BOOST_AUTO_TEST_CASE(Heterogenic_Dict) {
    std::stringstream str{ "d2:abi25e3:bcdli26e2:bbeee" };
    auto val = trrt::benc::parse_bencoding(str);
    trrt::benc::BencDict real{};
    real.val["ab"] = trrt::make_deep<trrt::benc::BencVal>(25);
    trrt::benc::BencList list = { { trrt::benc::BencInt{ 26 },
                                    trrt::benc::BencString{ "bb" } } };
    real.val["bcd"] = trrt::make_deep<trrt::benc::BencVal>(std::move(list));
    BOOST_CHECK(std::get<trrt::benc::BencDict>(val) == real);
};

BOOST_AUTO_TEST_CASE(Empty_Dict) {
    std::stringstream str{ "de" };
    auto val = trrt::benc::parse_bencoding(str);
    trrt::benc::BencDict real{};
    BOOST_CHECK(std::get<trrt::benc::BencDict>(val) == real);
};

BOOST_AUTO_TEST_CASE(Nested_Dict) {
    std::stringstream str{ "d1:ali25ee1:bd1:ci100eee" };
    auto val = trrt::benc::parse_bencoding(str);
    trrt::benc::BencDict real{};
    real.val["a"] = trrt::make_deep<trrt::benc::BencVal>(trrt::benc::BencList{ { 25 } });
    trrt::benc::BencDict temp{};
    temp.val["c"] = trrt::make_deep<trrt::benc::BencVal>(100);
    real.val["b"] = trrt::make_deep<trrt::benc::BencVal>(std::move(temp));
    BOOST_CHECK(std::get<trrt::benc::BencDict>(val) == real);
};