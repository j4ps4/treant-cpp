#pragma once

#include <tuple>
#include <list>
#include <string>
#include <filesystem>

#include "result.hpp"
#include "util.h"
#include "DF2/DF_util.h"

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
    int get_cost() const noexcept { return cost_; }
    size_t get_target_feature() const noexcept { return std::get<0>(post_condition_); }
    std::pair<double,double> get_pre_interval() const noexcept
    {
        return std::make_pair(
            std::get<1>(pre_conditions_), 
            std::get<2>(pre_conditions_)
        );
    }
    template<size_t N>
    bool is_applicable(const Row<N>& row) const noexcept;

    template<size_t N>
    Row<N> apply(const Row<N>& row) const noexcept;

    std::string debug_str() const;
private:
    const std::tuple<size_t, double, double> pre_conditions_;
    const std::pair<size_t, double> post_condition_;
    const int cost_;
    const bool is_numerical_;
};

using AttkList = std::list<AttackerRule>;

cpp::result<AttkList, std::string> 
load_attack_rules(const std::filesystem::path& fn);

template<size_t N>
bool AttackerRule::is_applicable(const Row<N>& row) const noexcept
{
    auto left = std::get<1>(pre_conditions_);
    auto right = std::get<2>(pre_conditions_);
    const auto feature = row(get_target_feature());
    auto res = left <= feature && feature <= right;
    return res;
}

template<size_t N>
Row<N> AttackerRule::apply(const Row<N>& row) const noexcept
{
    auto attack = std::get<1>(post_condition_);
    Row<N> newrow = row;
    newrow(get_target_feature()) += attack;
    return newrow;
}
