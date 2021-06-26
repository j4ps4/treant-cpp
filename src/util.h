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

template<typename T, typename... Ts>
const T& tuple_index(const std::tuple<Ts...>& tupl, size_t idx)
{
    switch(idx)
    {
        case 0:
            return std::get<0>(tupl);
        case 1:
            return std::get<1>(tupl);
        case 2:
            return std::get<2>(tupl);
        case 3:
            return std::get<3>(tupl);
        case 4:
            return std::get<4>(tupl);
        case 5:
            return std::get<5>(tupl);
        case 6:
            return std::get<6>(tupl);
        case 7:
            return std::get<7>(tupl);
        case 8:
            return std::get<8>(tupl);
        case 9:
            return std::get<9>(tupl);
        case 10:
            return std::get<10>(tupl);
        case 11:
            return std::get<11>(tupl);
        case 12:
            return std::get<12>(tupl);
        case 13:
            return std::get<13>(tupl);
        case 14:
            return std::get<14>(tupl);
        case 15:
            return std::get<15>(tupl);
        case 16:
            return std::get<16>(tupl);
        case 17:
            return std::get<17>(tupl);
        case 18:
            return std::get<18>(tupl);
        case 19:
            return std::get<19>(tupl);
        case 20:
            return std::get<20>(tupl);
        case 21:
            return std::get<21>(tupl);
        case 22:
            return std::get<22>(tupl);
        case 23:
            return std::get<23>(tupl);
    }
    throw std::out_of_range("tuple_index");
}

template<typename T, typename... Ts>
T& tuple_ref(std::tuple<Ts...>& tupl, size_t idx)
{
    switch(idx)
    {
        case 0:
            return std::get<0>(tupl);
        case 1:
            return std::get<1>(tupl);
        case 2:
            return std::get<2>(tupl);
        case 3:
            return std::get<3>(tupl);
        case 4:
            return std::get<4>(tupl);
        case 5:
            return std::get<5>(tupl);
        case 6:
            return std::get<6>(tupl);
        case 7:
            return std::get<7>(tupl);
        case 8:
            return std::get<8>(tupl);
        case 9:
            return std::get<9>(tupl);
        case 10:
            return std::get<10>(tupl);
        case 11:
            return std::get<11>(tupl);
        // case 12:
        //     return std::get<12>(tupl);
        // case 13:
        //     return std::get<13>(tupl);
        // case 14:
        //     return std::get<14>(tupl);
        // case 15:
        //     return std::get<15>(tupl);
        // case 16:
        //     return std::get<16>(tupl);
        // case 17:
        //     return std::get<17>(tupl);
        // case 18:
        //     return std::get<18>(tupl);
        // case 19:
        //     return std::get<19>(tupl);
        // case 20:
        //     return std::get<20>(tupl);
        // case 21:
        //     return std::get<21>(tupl);
        // case 22:
        //     return std::get<22>(tupl);
        // case 23:
        //     return std::get<23>(tupl);
    }
    throw std::out_of_range("tuple_index");
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