
#pragma once
#include <concepts>
#include <iterator>
#include <ranges>
namespace trrt {
template <typename R, typename T>
concept RangeOfConvertible =
std::ranges::range<R> && std::is_convertible_v<std::ranges::range_value_t<R>, T>;

template <typename R>
concept RangeOfIntegral =
std::ranges::range<R> && std::integral<std::ranges::range_value_t<R>>;

} // namespace trrt