
#pragma once

#include "utils/pointers.h"
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace trrt::benc {

struct BencVal;

struct BencInt {
    int64_t val;
    BencInt(int64_t num) : val{ num } {}

    auto operator<=>(const BencInt& other) const = default;
};

struct BencString {
    std::string val;
    BencString(std::string str) : val{ std::move(str) } {}
    BencString(const char* str) : val{ str } {}

    auto operator<=>(const BencString& other) const = default;
};


struct BencList {
    std::vector<BencVal> val;

    bool operator==(const BencList& other) const = default;
};

struct BencDict {
    std::unordered_map<std::string, trrt::deep_ptr<BencVal>> val;
    bool operator==(const BencDict& other) const = default;
};


struct BencVal : public std::variant<BencInt, BencString, BencList, BencDict> {
    using variant::variant;
};

/*
@brief Prints human-readable representation of BencVal tree in [out]
*/

void print_human_readable(const BencVal& val, std::ostream& out, int position = 0);


} // namespace trrt::benc
