#pragma once

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <vector>
#include <sstream>
#include <iomanip>

namespace Util {

template<typename... Args>
[[noreturn]] void die(const char* msg, Args... args)
{
    fmt::print(stderr, fg(fmt::color::crimson)|fmt::emphasis::bold, "error: ");
    fmt::print(stderr, msg, args...);
    fmt::print(stderr, "\n");
    exit(1);
}

template<typename... Args>
void warn(const char* msg, Args... args)
{
    fmt::print(fg(fmt::color::yellow)|fmt::emphasis::bold, "warning: ");
    fmt::print(msg, args...);
    fmt::print("\n");
}

template<typename... Args>
void info(const char* msg, Args... args)
{
    fmt::print(fg(fmt::color::sky_blue)|fmt::emphasis::bold, "info: ");
    fmt::print(msg, args...);
    fmt::print("\n");
}

template<typename... Args>
void log(const char* msg, Args... args)
{
    fmt::print(msg, args...);
    fmt::print("\n");
}


template<typename T>
auto numeral(T arg) { return +arg; }

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
            const auto& elem = vec[i];
            fmts << std::setprecision(12) << elem;
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

// template<typename K, typename V>
// struct fmt::formatter<std::map<K,V>>
// {

//     constexpr auto parse(format_parse_context& ctx)
//     {
//         return ctx.end();
//     }

//     template<typename FormatContext>
//     auto format(const std::map<K,V>& map, FormatContext& ctx)
//     {
//         std::stringstream fmts;
//         fmts << "{ ";
//         for (const auto& [key, value] : map)
//         {
//             fmts << key << ": " << value << "; ";
//         }
//         fmts << "}";
            
//         return format_to(
//            ctx.out(),
//            "{}",
//            fmts.str()
//         );
//     }
// };


template<typename F, typename S>
std::ostream& operator<<(std::ostream& os, const std::pair<F,S>& pair)
{
    os << "(";
    os << pair.first << ", " << pair.second;
    os << ")";
    return os;
}

namespace
{

template <typename... Ts, size_t... Is>
std::ostream& print_helper(std::ostream& os,
                const std::tuple<Ts...>& tuple,
                std::index_sequence<Is...>)
{
    ((os << Util::numeral(std::get<Is>(tuple)) << ','), ...);
    return os;
}

}

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& tuple)
{
    using Indices = std::index_sequence_for<Ts...>;
    print_helper(os, tuple, Indices{});
    return os;
}

template<typename... Ts>
struct fmt::formatter<std::tuple<Ts...>>
{

    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const std::tuple<Ts...>& tuple, FormatContext& ctx)
    {
        std::stringstream fmts;
        fmts << "(";
        fmts << tuple;
        fmts << ")";
            
        return format_to(
           ctx.out(),
           "{}",
           fmts.str()
        );
    }
};