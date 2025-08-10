
#pragma once

#include "utils/pointers.h"
#include <cstdint>
#include <ostream>
#include <stdexcept>
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
    operator int64_t() const { return val; }
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
// TODO(dimankarp): Think of whether private inheritance would be the same but more concise

struct BencDict {

    std::unordered_map<std::string, trrt::deep_ptr<BencVal>> val;

    using ConstIterator =
    std::unordered_map<std::string, trrt::deep_ptr<BencVal>>::const_iterator;

    bool operator==(const BencDict& other) const = default;
    trrt::deep_ptr<BencVal>& at(const std::string& key) & {
        try {
            return val.at(key);
        } catch(std::out_of_range& e) {
            throw std::out_of_range{ std::string(e.what()) + " key: " + key };
        }
    }
    trrt::deep_ptr<BencVal>&& at(const std::string& key) && {
        try {
            return std::move(val.at(key));
        } catch(std::out_of_range& e) {
            throw std::out_of_range{ std::string(e.what()) + " key: " + key };
        }
    }
    const trrt::deep_ptr<BencVal>& at(const std::string& key) const& {
        try {
            return val.at(key);
        } catch(std::out_of_range& e) {
            throw std::out_of_range{ std::string(e.what()) + " key: " + key };
        }
    }

    ConstIterator find(const std::string& key) const { return val.find(key); }
    ConstIterator begin() const { return val.begin(); }
    ConstIterator end() const { return val.end(); }
};


struct BencVal : public std::variant<BencInt, BencString, BencList, BencDict> {
    using variant::variant;

    /**
      \defgroup BencGetFuncs Quick std::get(...) for BencVals

      @throw std::bad_variant_access same as in std::get
      @{
     */

    BencInt& get_int() { return std::get<BencInt>(*this); }

    const BencInt& get_int() const { return std::get<BencInt>(*this); }

    BencString& get_str() { return std::get<BencString>(*this); }

    const BencString& get_str() const { return std::get<BencString>(*this); }

    BencList& get_list() { return std::get<BencList>(*this); }

    const BencList& get_list() const { return std::get<BencList>(*this); }

    BencDict& get_dict() { return std::get<BencDict>(*this); }

    const BencDict& get_dict() const { return std::get<BencDict>(*this); }
    /** @} */
};

/// Prints human-readable representation of BencVal.
/**
    @param position Starting identation in tabs.
*/
void print_human_readable(const BencVal& val, std::ostream& out, int position = 0);


} // namespace trrt::benc
