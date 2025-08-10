#include "utils/concepts.h"
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <ranges>
namespace trrt::serial {


/**

    Write @ref T value to @ref start iterator
   in @b network order bytes.

   @note @ref start is iterated by sizeof(T)

    @tparam T std::integral
    @tparam C convertible from char
    @tparam OutIt trrt::ConvertibleTypeOutputIterator<char, C>

 */
template <std::integral T, std::output_iterator<char> OutIt>
inline void write_netord_impl(T value, OutIt& start) {
    static const int BITS_IN_BYTE = 8;
    static const unsigned char BYTE_MASK = 0xff;
    int shift = sizeof((value)-1) * BITS_IN_BYTE;
    for(int i = 0; i < static_cast<int>(sizeof(T)); ++i) {
        char b = (value >> shift) & BYTE_MASK;
        *start = b;
        ++start;
        shift -= BITS_IN_BYTE;
    }
}

template <typename OutIt> void write_uint8(uint8_t val, OutIt& start) {
    write_netord_impl<uint8_t>(val, start);
}


template <typename OutIt> void write_uint16(uint16_t val, OutIt& start) {
    write_netord_impl<uint16_t>(val, start);
}

template <typename OutIt> void write_uint32(uint32_t val, OutIt& start) {
    write_netord_impl<uint32_t>(val, start);
}

template <RangeOfIntegral R, typename OutIt>
void write_range(const R range, OutIt& start) {
    std::ranges::for_each(range, [&](auto x) {
        write_netord_impl<std::ranges::range_value_t<R>>(x, start);
    });
}


} // namespace trrt::serial