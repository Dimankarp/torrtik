#pragma once

#include "serial/read.h"
#include <cstdint>
#include <iterator>
#include <message.h>
#include <serial/write.h>
#include <variant>
namespace trrt::message {

namespace serial = trrt::serial;


template <std::input_iterator InIt, typename... Msgs>
std::variant<Msgs...> deserialize(InIt& it, const InIt end) {

    using MsgVar = std::variant<Msgs...>;
    using Deserializer = MsgVar (*)(InIt&, const InIt);

    constexpr std::size_t MAX_ID = std::max({Msgs::msg_id...});
    constexpr std::array<Deserializer, MAX_ID + 1> ftable = []() {
        std::array<Deserializer, MAX_ID + 1> arr{};
        ((arr[Msgs::msg_id] = [](InIt& it, const InIt end) -> MsgVar {
             return Msgs::deserialize_msg(it, end);
         }),
         ...);
        return arr;
    }();

    const std::uint8_t msg_id = serial::read_uint8(it);
    return ftable.at(msg_id)(it, end);
}


} // namespace trrt::message