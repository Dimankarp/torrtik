
#include "bencoding/benctypes.h"
#include <algorithm>
namespace trrt::benc {

namespace {
void print_int(const BencInt& val, std::ostream& out, int tab) {
    out << std::string(tab, '\t');
    out << "Int: " << val.val << '\n';
}

void print_str(const BencString& val, std::ostream& out, int tab) {
    out << std::string(tab, '\t');
    out << "Str: " << val.val << '\n';
}

void print_list(const BencList& val, std::ostream& out, int tab) {
    auto tabs = std::string(tab, '\t');
    out << tabs << "[" << '\n';
    tab++;
    std::ranges::for_each(val.val,
                          [&](auto& x) { print_human_readable(x, out, tab); });
    out << tabs << "]" << '\n';
}

void print_dict_pair(const BencString& key, const BencVal& val, std::ostream& out, int tab) {
    auto tabs = std::string(tab, '\t');
    out << tabs << "Key: " << key.val << '\n';
    out << tabs << "Value: " << '\n';
    print_human_readable(val, out, tab + 1);
}

void print_dict(const BencDict& val, std::ostream& out, int tab) {
    auto tabs = std::string(tab, '\t');
    out << tabs << "{" << '\n';
    tab++;
    BencString str = std::string{ "aadad" };
    std::ranges::for_each(val.val, [&](auto& x) {
        print_dict_pair(x.first, *x.second, out, tab);
    });
    out << tabs << "}" << '\n';
}


} // namespace

void print_human_readable(const BencVal& val, std::ostream& out, int position) {
    if(const auto* s = std::get_if<trrt::benc::BencString>(&val)) {
        print_str(*s, out, position);
    } else if(const auto* i = std::get_if<trrt::benc::BencInt>(&val)) {
        print_int(*i, out, position);
    } else if(const auto* l = std::get_if<trrt::benc::BencList>(&val)) {
        print_list(*l, out, position);
    } else if(const auto* d = std::get_if<trrt::benc::BencDict>(&val)) {
        print_dict(*d, out, position);
    }
}


} // namespace trrt::benc
