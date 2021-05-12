#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <vector>
#include <sstream>

namespace Util {

template<typename... Args>
[[noreturn]] void die(const char* msg, Args... args)
{
    fmt::print(stderr, msg, args...);
    exit(1);
}

}

template<typename T>
struct fmt::formatter<std::vector<T>>
{

    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const std::vector<T>& vec, FormatContext& ctx)
    {
        std::stringstream fmts;
        fmts << "[";
        for (std::size_t i = 0; i < vec.size()-1; i++)
        {
            auto& elem = vec[i];
            fmts << elem;
            fmts << ", ";
        }
        fmts << vec[vec.size()-1];
        fmts << "]";
            
        return format_to(
           ctx.out(),
           "{}",
           fmts.str()
        );
    }
};