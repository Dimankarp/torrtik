#include "bencoding/benc.h"
#include "bencoding/benctypes.h"
#include <algorithm>
#include <cassert>
#include <variant>
namespace trrt::benc {

namespace {


std::string bencode_int(const BencInt& val) {
    return "i" + std::to_string(val.val) + "e";
}

std::string bencode_str(const BencString& val) {
    return std::to_string(val.val.size()) + ":" + val.val;
}

std::string bencode_list(const BencList& val) {
    std::string result{ "l" };
    std::ranges::for_each(val.val,
                          [&](const auto& x) { result.append(bencode(x)); });
    result.append("e");
    return result;
}

std::string bencode_dict(const BencDict& val) {
    std::string result{ "d" };
    std::vector<std::pair<std::string, const deep_ptr<BencVal>*>> pairs{};
    pairs.reserve(val.val.size());

    for(const auto& item : val.val) {
        pairs.emplace_back(item.first, &item.second);
    }
    std::ranges::sort(pairs, [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    std::ranges::for_each(pairs, [&](const auto& pair) {
        result.append(bencode_str({ pair.first }));
        result.append(bencode(**pair.second)); // Either major or lieutenant pair.second
    });
    result.append("e");
    return result;
}


} // namespace


std::string bencode(const BencVal& val) {
    if(const auto* s = std::get_if<trrt::benc::BencString>(&val)) {
        return bencode_str(*s);
    }
    if(const auto* i = std::get_if<trrt::benc::BencInt>(&val)) {
        return bencode_int(*i);
    }
    if(const auto* l = std::get_if<trrt::benc::BencList>(&val)) {
        return bencode_list(*l);
    }
    if(const auto* d = std::get_if<trrt::benc::BencDict>(&val)) {
        return bencode_dict(*d);
    }
    assert(false); // Unreachable
}

} // namespace trrt::benc