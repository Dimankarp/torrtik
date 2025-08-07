
#pragma once
#include <ranges>
namespace trrt {
template <typename R, typename T>
concept RangeOfConvertible =
std::ranges::range<R> && std::is_convertible_v<std::ranges::range_value_t<R>, T>;


}