#pragma once

#include <boost/asio.hpp>
#include <format>
#include <iostream>
#include <source_location>

template <> struct std::formatter<boost::asio::ip::tcp::endpoint> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const boost::asio::ip::tcp::endpoint& ep, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}:{}", ep.address().to_string(), ep.port());
    }
};

namespace trrt {

enum class LogLevel : char { DEBUG = 0, INFO, WARNING, ERROR };

constexpr bool loggingEnabled() {

    // Could be more compelx logic here
#ifdef TRRT_ENABLE_LOGGING
    return true;
#else
    return false;
#endif
}


/// Wrappings for default level & loc values passed in void log(...)
struct LocLogMeta {
    LogLevel level = LogLevel::INFO;
    std::source_location loc;
};

struct LogMeta {
    LogLevel level = LogLevel::INFO;
};

constexpr LocLogMeta
loc_meta(LogLevel level = LogLevel::INFO,
         const std::source_location loc = std::source_location::current()) {
    return LocLogMeta{ .level = level, .loc = loc };
}

constexpr LogMeta meta(LogLevel level = LogLevel::INFO) {
    return LogMeta{ .level = level };
}


// TODO(dimankarp): Add logging settings
template <typename... Args>
void log(LocLogMeta meta, std::format_string<Args...> fmt, Args&&... args) {
    if constexpr(loggingEnabled()) {
        std::cerr
        << std::format("{}|{}|{}:{}|", meta.loc.file_name(),
                       meta.loc.function_name(), meta.loc.line(), meta.loc.column())
        << std::format(fmt, std::forward<decltype(args)>(args)...) << std::endl;
    }
}

template <typename... Args>
void log(LogMeta meta, std::format_string<Args...> fmt, Args&&... args) {
    if constexpr(loggingEnabled()) {
        std::cerr << std::format(fmt, std::forward<decltype(args)>(args)...) << std::endl;
    }
}

} // namespace trrt