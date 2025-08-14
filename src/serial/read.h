#pragma once

#include "utils/concepts.h"
#include <concepts>
#include <cstdint>
#include <iterator>
namespace trrt::serial {


/**

    Reads @ref T value in @b native-endiannes from @ref start
   @b network order bytes iterator.

   @note @ref start is iterated by sizeof(T)

    @tparam T std::integral the type to parse to
    @tparam InIt std::input_iterator

 */
template <std::integral T, std::input_iterator InIt>
inline T read_netord_impl(InIt& start) {

    if constexpr(sizeof(T) != 1) {
        static const int BITS_IN_BYTE = 8;
        T value = 0;
        for(int i = 0; i < static_cast<int>(sizeof(T)); ++i) {
            value <<= BITS_IN_BYTE;
            value |= static_cast<unsigned char>(*start);
            ++start;
        }
        return value;
    } else {
        // aoviding UB on opeartion(b << 8) for bytes
        return static_cast<T>(*start++);
    }
}

template <typename InIt> uint8_t read_uint8(InIt& start) {
    return read_netord_impl<uint8_t>(start);
}


template <typename InIt> uint16_t read_uint16(InIt& start) {
    return read_netord_impl<uint16_t>(start);
}

template <typename InIt> uint32_t read_uint32(InIt& start) {
    return read_netord_impl<uint32_t>(start);
}

template <RangeOfIntegral R, typename OutIt>
void read_range(R& range, OutIt& start) {
    auto it = range.begin();
    while(it != range.end()) {
        *it = read_netord_impl<std::ranges::range_value_t<R>>(start);
        it++;
    }
}


} // namespace trrt::serial