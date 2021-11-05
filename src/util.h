#pragma once

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <vector>
#include <set>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <mutex>
#include <random>

extern std::mutex print_mut;
extern int verbosity;

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
void err(const char* msg, Args... args)
{
    if (verbosity > 0)
    {
        fmt::print(stderr, fg(fmt::color::crimson)|fmt::emphasis::bold, "error: ");
        fmt::print(stderr, msg, args...);
        fmt::print(stderr, "\n");
    }
}

template<typename... Args>
void warn(const char* msg, Args... args)
{
    if (verbosity > 0)
    {
        std::unique_lock lock(print_mut);
        fmt::print(fg(fmt::color::yellow)|fmt::emphasis::bold, "warning: ");
        fmt::print(msg, args...);
        fmt::print("\n");
    }
}

template<typename... Args>
void info(const char* msg, Args... args)
{
    if (verbosity > 1)
    {
        std::unique_lock lock(print_mut);
        fmt::print(fg(fmt::color::sky_blue)|fmt::emphasis::bold, "info: ");
        fmt::print(msg, args...);
        fmt::print("\n");
    }
}

template<int verb, typename... Args>
void log(const char* msg, Args... args)
{
    if (verbosity >= verb)
    {
        std::unique_lock lock(print_mut);
        fmt::print(msg, args...);
        fmt::print("\n");
    }
}

template<typename T>
auto numeral(T arg) { return +arg; }

#define TIME std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - now).count()

inline std::string pretty_timediff(double diff)
{
    int hours = std::floor(diff / (60.0*60.0));
    double hsm = hours*60*60;
    double secs1 = diff-hsm;
    int mins = std::floor(secs1 / 60.0);
    double minss = mins*60;
    double secs2 = secs1-minss;
    return fmt::format("{}h {}m {:.2f}s", hours, mins, secs2);
}

template<size_t N>
std::set<size_t> feature_set()
{
    auto gen = [](){static size_t x = 0; return x++;};
    std::set<size_t> out;
    std::generate_n(std::inserter(out, out.begin()), N, gen);
    return out;
}

inline std::set<size_t> sample_blacklisted(const std::set<size_t>& all_features,
    const std::set<size_t>& blacklist, size_t n_sample, std::mt19937_64& rd, bool do_sampling = true)
{
    // 1. filter out any blacklisted features from the list of features actually considered
    std::set<size_t> not_bl;
    std::set_difference(all_features.begin(), all_features.end(),
        blacklist.begin(), blacklist.end(),
        std::inserter(not_bl, not_bl.begin()));
    if (!do_sampling)
        return not_bl;
    // 2. randomly sample a subset of n features out of the actual features
    size_t n_sample_prime = std::min(not_bl.size(), n_sample);
    std::set<size_t> actual_features;
    std::sample(not_bl.begin(), not_bl.end(), 
        std::inserter(actual_features, actual_features.begin()), 
        n_sample_prime, rd);
    return actual_features;
}

}


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