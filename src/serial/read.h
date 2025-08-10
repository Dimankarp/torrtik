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
    static const int BITS_IN_BYTE = 8;
    T value = 0;
    for(int i = 0; i < static_cast<int>(sizeof(T)); ++i) {
        value <<= BITS_IN_BYTE;
        value |= static_cast<unsigned char>(*start);
        ++start;
    }
    return value;
}

/// Partial specialization for aoviding UB on opeartion(b << 8) for bytes
template <std::input_iterator InIt>
inline std::uint8_t read_netord_impl(InIt& start) {
    return static_cast<std::uint8_t>(*start++);
}

template <std::input_iterator InIt>
inline std::int8_t read_netord_impl(InIt& start) {
    return static_cast<std::int8_t>(*start++);
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


} // namespace trrt::serial