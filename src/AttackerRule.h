#pragma once

#include <tuple>
#include <list>
#include <string>
#include <filesystem>
#include <set>

#include <cereal/types/tuple.hpp>

#include "result.hpp"
#include "util.h"
#include "DF2/DF_def.h"

enum class AttackType
{
    Normal,
    InfBall,
    Constant
};

class AttackerRule
{
public:
    AttackerRule(std::tuple<std::size_t, double, double> pre_conditions,
                 std::tuple<std::size_t, double> post_condition,
                 int cost,
                 bool is_numerical = true) :
                 pre_conditions_(pre_conditions),
                 post_condition_(post_condition),
                 cost_(cost),
                 is_numerical_(is_numerical)
                 {}
    
    AttackerRule() = default;

    int get_cost() const noexcept { return cost_; }
    size_t get_target_feature() const noexcept { return std::get<0>(post_condition_); }
    std::tuple<double,double> get_pre_interval() const noexcept
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

    double get_post() const noexcept {return std::get<1>(post_condition_);}

    std::string debug_str() const;

    template<typename Archive>
    void save(Archive& archive) const
    {
        archive(pre_conditions_, post_condition_, cost_, is_numerical_);
    }

    template<typename Archive>
    void load(Archive& archive)
    {
        archive(pre_conditions_, post_condition_, cost_, is_numerical_);
    }
private:
    std::tuple<size_t, double, double> pre_conditions_;
    std::tuple<size_t, double> post_condition_;
    int cost_;
    bool is_numerical_;
};

using AttkList = std::vector<AttackerRule>;
using LoadType = std::tuple<AttkList, AttackType>;

cpp::result<LoadType, std::string> 
load_attack_rules(const std::filesystem::path& fn, const std::map<std::string,
    size_t>& column_map, const std::set<size_t>& id_set, double epsilon);

template<size_t N>
bool AttackerRule::is_applicable(const Row<N>& row) const noexcept
{
    return true;
    // auto left = std::get<1>(pre_conditions_);
    // auto right = std::get<2>(pre_conditions_);
    // const auto feature = row(get_target_feature());
    // auto res = left <= feature && feature <= right;
    // return res;
}

template<size_t N>
Row<N> AttackerRule::apply(const Row<N>& row) const noexcept
{
    auto attack = std::get<1>(post_condition_);
    Row<N> newrow = row;
    newrow(get_target_feature()) += attack;
    return newrow;
}
