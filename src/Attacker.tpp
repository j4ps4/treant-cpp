#include <deque>
#include <algorithm>
#include <iterator>
#include <array>
#include <iostream>

#include <fmt/core.h>
// #include <cereal/types/unordered_map.hpp>
// #include <cereal/types/vector.hpp>
// #include <cereal/types/tuple.hpp>
// #include <cereal/archives/binary.hpp>
#include <fstream>

#include "util.h"

template<size_t N>
void Attacker<N>::print_rules() const
{
    fmt::print("type: {}\n", type_ == AttackType::Normal ? "normal" : "inf_ball");
    for (const auto& r : rules_)
        fmt::print("{}\n", r.debug_str());
}

template<size_t NX>
void Attacker<NX>::compute_target_features()
{
    for (const auto& r : rules_)
    {
        auto f = r.get_target_feature();
        features_.insert(f);
    }
}

template<size_t NX>
std::tuple<CostArr,CostArr> Attacker<NX>::push_costs(const Row<NX>& x, size_t feature_id,
    double split_value, int spent) const
{
    CostArr push_left = {-1,-1};
    CostArr push_right = {-1,-1};
    size_t left_idx = 0;
    size_t right_idx = 0;
    const double feature = x[feature_id];
    const bool is_left = feature <= split_value;
    // No cost needed
    if (is_left)
        push_left[left_idx++] = spent;
    else
        push_right[right_idx++] = spent;
    
    if (spent >= budget_)
    {
        return std::make_tuple(push_left, push_right);
    }

    const double deform = is_constant_ ? flat_deform_ : deformations_.at(feature_id);
    if (deform == 0.0)
    {
        return std::make_tuple(push_left, push_right);
    }
    // crosses the threshold to left
    if (feature - deform <= split_value)
    {
        push_left[left_idx++] = spent+1;
    }
    // crosses the threshold to right
    if (feature + deform > split_value)
    {
        push_right[right_idx++] = spent+1;
    }
    return std::make_tuple(push_left, push_right);
}

template<size_t NX>
TupleArr<NX, 2> Attacker<NX>::adjacent_attack(const Row<NX>& x, size_t feature_id,
    int spent) const
{
    TupleArr<NX, 2> out;
    if (spent >= budget_)
    {
        out[0] = std::make_tuple(x, -1);
        return out;
    }
    if (is_constant_)
    {
        Row<NX> x_p1 = x;
        x_p1[feature_id] += flat_deform_;
        out[0] = std::make_tuple(x_p1, spent+1);
        Row<NX> x_p2 = x;
        x_p2[feature_id] -= flat_deform_;
        out[1] = std::make_tuple(x_p2, spent+1);
        return out;
    }
    else
    {
        const double deform = deformations_.at(feature_id);
        Row<NX> x_p1 = x;
        x_p1[feature_id] += deform;
        out[0] = std::make_tuple(x_p1, spent+1);
        Row<NX> x_p2 = x;
        x_p2[feature_id] -= deform;
        out[1] = std::make_tuple(x_p2, spent+1);
        return out;
    }
}

template<size_t NX>
void Attacker<NX>::compute_deformations()
{
    for (auto fid : features_)
    {
        if (is_constant_)
            deformations_[fid] = flat_deform_;
        else
        {
            auto rule_it = std::find_if(rules_.cbegin(), rules_.cend(), [fid](const auto& rule){
                    return rule.get_target_feature() == fid;
            });
            if (rule_it == rules_.cend())
                continue;
            double deform = std::abs(rule_it->get_post());
            deformations_[fid] = deform;
        }
    }
    // fill with zeroes
    const auto all_feats = Util::feature_set<NX>();
    for (auto fid : all_feats)
    {
        if (!deformations_.contains(fid))
            deformations_[fid] = 0.0;
    }
}

template<size_t NX>
void Attacker<NX>::remove_useless_rules()
{
    auto it = std::remove_if(rules_.begin(), rules_.end(), [&](auto& rule){
        return !features_.contains(rule.get_target_feature());
    });
    rules_.resize(std::distance(rules_.begin(), it));
}
