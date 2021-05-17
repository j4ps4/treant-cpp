#pragma once

#include <tuple>
#include <list>
#include <string>
#include <variant>

#include "def.h"

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
    bool is_applicable(const DF& row) const;
    void apply(DF& row) const;
private:
    std::tuple<size_t, double, double> pre_conditions_;
    std::pair<size_t, double> post_condition_;
    int cost_;
    bool is_numerical_;
};

std::variant<std::list<AttackerRule>, std::string> 
load_attack_rules(const std::string& fn, const ColMap& colmap);