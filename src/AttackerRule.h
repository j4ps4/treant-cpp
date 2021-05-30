#pragma once

#include <tuple>
#include <list>
#include <string>
#include <variant>

#include "result.hpp"
#include "util.h"

#define ATTK_TYPES double,float,uint32_t,int32_t,uint8_t,int8_t,bool

class AttackerRule
{
public:
    AttackerRule(std::tuple<std::size_t, std::variant<ATTK_TYPES>, std::variant<ATTK_TYPES>> pre_conditions,
                 std::pair<std::size_t, std::variant<ATTK_TYPES>> post_condition,
                 int cost,
                 bool is_numerical = true) :
                 pre_conditions_(pre_conditions),
                 post_condition_(post_condition),
                 cost_(cost),
                 is_numerical_(is_numerical)
                 {}
    int get_cost() const noexcept { return cost_; }
    size_t get_target_feature() const noexcept { return std::get<0>(post_condition_); }
    template<typename F>
    std::pair<F,F> get_pre_interval() const noexcept
    {
        return std::make_pair(
            std::get<F>(std::get<1>(pre_conditions_)), 
            std::get<F>(std::get<2>(pre_conditions_))
        );
    }
    template<size_t I, typename F, typename... Ts>
    bool is_applicable(const std::tuple<Ts...>& row) const noexcept;
    template<size_t I, typename F, typename... Ts>
    std::tuple<Ts...> apply(const std::tuple<Ts...>& row) const noexcept;
    template<typename F>
    std::string debug_str() const;
private:
    const std::tuple<size_t, std::variant<ATTK_TYPES>, std::variant<ATTK_TYPES>> pre_conditions_;
    const std::pair<size_t, std::variant<ATTK_TYPES>> post_condition_;
    const int cost_;
    const bool is_numerical_;
};

using AttkList = std::list<AttackerRule>;

template<typename... Fs>
cpp::result<AttkList, std::string> 
load_attack_rules(const std::string& fn);

template<size_t I, typename F, typename... Ts>
bool AttackerRule::is_applicable(const std::tuple<Ts...>& row) const noexcept
{
    auto left = std::get<F>(std::get<1>(pre_conditions_));
    auto right = std::get<F>(std::get<2>(pre_conditions_));
    const auto& feature = std::get<I>(row);
    auto res = left <= feature && feature <= right;
    return res;
}

template<size_t I, typename F, typename... Ts>
std::tuple<Ts...> AttackerRule::apply(const std::tuple<Ts...>& row) const noexcept
{
    auto attack = std::get<F>(std::get<1>(post_condition_));
    std::tuple<Ts...> newrow = row;
    auto& loc = std::get<I>(newrow);
    loc += attack;
    return newrow;
}

#include "AttackerRule.tpp"