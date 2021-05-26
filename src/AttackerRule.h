#pragma once

#include <tuple>
#include <list>
#include <string>
#include <variant>

#include "result.hpp"
#include "util.h"

class AttackerRule
{
public:
    AttackerRule(std::tuple<std::size_t, double, double> pre_conditions,
                 std::pair<std::size_t, double> post_condition,
                 int cost,
                 bool is_numerical = true) :
                 pre_conditions_(pre_conditions),
                 post_condition_(post_condition),
                 cost_(cost),
                 is_numerical_(is_numerical)
                 {}
    int get_cost() const { return cost_; }
    size_t get_target_feature() const { return std::get<0>(post_condition_); }
    std::pair<double,double> get_pre_interval() const
    {
        return std::make_pair(std::get<1>(pre_conditions_), std::get<2>(pre_conditions_));
    }
    template<typename... Ts>
    bool is_applicable(const std::tuple<Ts...>& row) const;
    template<typename... Ts>
    std::tuple<Ts...> apply(const std::tuple<Ts...>& row) const;
    std::string debug_str() const;
private:
    std::tuple<size_t, double, double> pre_conditions_;
    std::pair<size_t, double> post_condition_;
    int cost_;
    bool is_numerical_;
};

using AttkList = std::list<AttackerRule>;

cpp::result<AttkList, std::string> 
load_attack_rules(const std::string& fn);

template<typename... Ts>
bool AttackerRule::is_applicable(const std::tuple<Ts...>& row) const
{
    auto feature_id = std::get<0>(pre_conditions_);
    auto left = std::get<1>(pre_conditions_);
    auto right = std::get<2>(pre_conditions_);
    auto feature = Util::tuple_index<double>(row, feature_id);
    auto res = left <= feature && feature <= right;
    return res;
}

template<typename... Ts>
std::tuple<Ts...> AttackerRule::apply(const std::tuple<Ts...>& row) const
{
    auto feature_id = std::get<0>(post_condition_);
    auto attack = std::get<1>(post_condition_);
    std::tuple<Ts...> newrow = row;
    double* loc = Util::tuple_ref<double>(newrow, feature_id);
    *loc += attack;
    return newrow;
}