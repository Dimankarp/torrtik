
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include <cctype>
#include <memory>
#include <stdexcept>
namespace trrt::benc {

namespace {


char next_char(std::istream& in) {
    int i = in.get();
    if(in) {
        return static_cast<char>(i);
    }
    throw std::runtime_error(
    "Bencoding parse failure: failed to read from input stream");
}

bool next_if_equals(char expected, std::istream& in) {
    char c = next_char(in);
    if(c != expected) {
        in.putback(c);
        return false;
    }
    return true;
}


BencInt parse_int(std::istream& in, std::string& buf) {
    for(char c = next_char(in); c != 'e'; c = next_char(in)) {
        buf += c;
    }
    try {
        int64_t val = std::stoll(buf);
        buf.clear();
        return { val };
    } catch(std::invalid_argument& a) {
        throw std::runtime_error{ "Bencoding parse failure: failed to "
                                  "interpret int" };
    }
}

BencString parse_str(std::istream& in, std::string& buf) {
    for(char c = next_char(in); c != ':'; c = next_char(in)) {
        if(isalpha(c) != 0)
            throw std::runtime_error(
            "Bencoding parse failure: alpha chars in string length");
        buf += c;
    }
    int64_t len = std::stoll(buf);
    buf.clear();

    for(int i = 0; i < len; i++) {
        buf += next_char(in);
    }

    BencString result{ buf };
    buf.clear();

    return result;
}

BencVal parse_next(std::istream& in, std::string& buf);

// NOLINTNEXTLINE(misc-no-recursion)
BencList parse_list(std::istream& in, std::string& buf) {
    BencList list{};
    while(!next_if_equals('e', in)) {
        list.val.push_back(parse_next(in, buf));
    }
    return list;
}

// NOLINTNEXTLINE(misc-no-recursion)
BencDict parse_dict(std::istream& in, std::string& buf) {
    BencDict dict{};
    while(!next_if_equals('e', in)) {
        BencString key = parse_str(in, buf);
        BencVal val = parse_next(in, buf);
        dict.val[key.val] = trrt::make_deep<BencVal>(std::move(val));
    }
    return dict;
}


// NOLINTNEXTLINE(misc-no-recursion)
BencVal parse_next(std::istream& in, std::string& buf) {
    char c = next_char(in);
    switch(c) {
    case 'i': return parse_int(in, buf);
    case 'l': return parse_list(in, buf);
    case 'd': return parse_dict(in, buf);
    default: {
        buf.push_back(c);
        return parse_str(in, buf);
    }
    }
}
} // namespace

BencVal parse_bencoding(std::istream& in) {
    std::string buf{};
    return parse_next(in, buf);
}

} // namespace trrt::benc