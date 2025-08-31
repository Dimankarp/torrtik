#pragma once

namespace trrt {

template <typename... Base> struct Visitor : public Base... {
    using Base::operator()...;
};

} // namespace trrt