#pragma once

#include <tuple>
#include <list>
#include <string>
#include <variant>

#include "DF.h"
#include "result.hpp"

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
    bool is_applicable(const DFRView& row) const;
    DFR apply(const DFRView& row) const;
    bool is_applicable(const DFR& row) const;
    DFR apply(const DFR& row) const;
    std::string debug_str() const;
private:
    std::tuple<size_t, double, double> pre_conditions_;
    std::pair<size_t, double> post_condition_;
    int cost_;
    bool is_numerical_;
};

using AttkList = std::list<AttackerRule>;

cpp::result<AttkList, std::string> 
load_attack_rules(const std::string& fn, const ColMap& colmap);