#pragma once

namespace trrt {

template <typename... Base> class Visitor : public Base... {
    using Base::operator()...;
};

} // namespace trrt