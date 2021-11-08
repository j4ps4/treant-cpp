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

namespace 
{

template<size_t N>
bool is_equal_perturbation(const PairT<N>& x, const PairT<N>& y)
{
    return (std::get<0>(x) == std::get<0>(y)).all();
        // && std::get<1>(x) <= std::get<1>(y);
}

template<size_t N>
bool check_equal_perturbation(const TupleVec<N>& attacks, const PairT<N>& y)
{
    for (size_t i = 0; i < attacks.size(); i++)
    {
        auto& row = attacks[i];
        if (is_equal_perturbation<N>(row, y))
        {
            return true;
        }
    }
    return false;
}

template<size_t N>
void compute_normal(const TupleVec<N>& attacks, const AttkList& rules, std::deque<PairT<N>>& queue,
    const Row<N>& inst, int total_budget, int running_cost, size_t feature_id)
{
    for (const auto& rule : rules)
    {
    if (rule.get_target_feature() == feature_id && rule.is_applicable<N>(inst))
    {
        if (total_budget >= running_cost + rule.get_cost())
        {
            auto x_prime = rule.apply<N>(inst);
            auto cost_prime = running_cost + rule.get_cost();
            auto pair = std::make_tuple(x_prime, cost_prime);
            if (!check_equal_perturbation<N>(attacks, pair))
            {
                queue.push_front(pair);
            }
            std::array<double,2> sarr = {inst(feature_id), x_prime(feature_id)};
            std::sort(sarr.begin(), sarr.end());
            auto [low, high] = sarr;
            auto [pre1, pre2] = rule.get_pre_interval();
            std::vector<double> z;
            if (low < pre1 && pre1 < high)
                z.push_back(pre1);
            if (low < pre2 && pre2 < high)
                z.push_back(pre2);
            for (double zi : z)
            {
                x_prime = inst;
                x_prime(feature_id) = zi;
                auto pair = std::make_tuple(x_prime, cost_prime);
                if (!check_equal_perturbation<N>(attacks, pair))
                    queue.push_front(pair);
            }
        }
    }
    }
}

template<size_t N>
void compute_infball(TupleVec<N>& attacks, const AttackerRule& rule,
    const Row<N>& inst, int total_budget, int running_cost, size_t feature_id)
{
    if (rule.get_target_feature() == feature_id && rule.is_applicable<N>(inst))
    {
        if (total_budget >= running_cost + rule.get_cost())
        {
            auto x_prime = rule.apply<N>(inst);
            auto cost_prime = running_cost + rule.get_cost();
            auto pair = std::make_tuple(x_prime, cost_prime);
            if (!check_equal_perturbation<N>(attacks, pair))
            {
                attacks.push_back(pair);
            }
        }
    }
}

}

template<size_t N>
TupleVec<N> Attacker<N>::compute_attack(const Row<N>& rw, size_t feature_id, int cost) const
{
    TupleVec<N> attacks;
    switch (type_)
    {
    case AttackType::Constant:
    {
        Util::die("called compute_attack with Constant attack!!");
    }
    case AttackType::Normal:  
    {
        std::deque<PairT<N>> queue = {{rw, cost}};

        while (queue.size() > 0)
        {
            auto [inst, budget] = queue.back();
            queue.pop_back();
            attacks.push_back(std::make_tuple(inst, budget));
            compute_normal<N>(attacks, rules_, queue, inst, budget_, budget, feature_id);
        }
        break;
    }
    case AttackType::InfBall:
    {
        attacks.push_back(std::make_tuple(rw, cost));
        for (const auto& rule : rules_)        
            compute_infball<N>(attacks, rule, rw, budget_, cost, feature_id);
    }
    }
    return attacks;
}

template<size_t N>
void Attacker<N>::compute_attacks(const DF<N>& X) const
{
    return;
}

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
TupleVec<NX> Attacker<NX>::max_attack(const Row<NX>& x, size_t feature_id) const
{

    if (features_.contains(feature_id))
    {
        // auto attack_key = std::make_tuple(x, feature_id);
        // if (!attacks_.contains(attack_key))
        //     attacks_[attack_key] = compute_attack(x, feature_id, 0);

        // auto& attacks_xf = attacks_.at(attack_key);
        auto attacks_xf = compute_attack(x, feature_id, 0);
        TupleVec<NX> out;
        auto max_cost_it = std::max_element(attacks_xf.begin(), attacks_xf.end(), [](const auto& pair1, const auto& pair2){
            return std::get<1>(pair1) < std::get<1>(pair2);
        });
        auto max_cost = std::get<1>(*max_cost_it);
        std::copy_if(attacks_xf.begin(), attacks_xf.end(), std::back_inserter(out), [max_cost](auto& pair){
            auto c = std::get<1>(pair); return c == 0 || c == max_cost;
        });
        
        return out;
    }
    else
    {
        TupleVec<NX> out{{x, 0}};
        return out;
    }

}

template<size_t NX>
std::tuple<TupleArr<NX, 3>, std::ptrdiff_t> Attacker<NX>::attack(const Row<NX>& x, size_t feature_id,
    int spent) const
{
    TupleArr<NX, 3> out;
    out[0] = std::make_tuple(x, spent);
    if (spent >= budget_)
    {
        return std::make_tuple(out, 1);
    }
    if (is_constant_)
    {
        if (flat_deform_ == 0.0)
        {
            return std::make_tuple(out, 1);
        }
        Row<NX> x_p1 = x;
        x_p1[feature_id] += flat_deform_;
        out[1] = std::make_tuple(x_p1, spent+1);
        Row<NX> x_p2 = x;
        x_p2[feature_id] -= flat_deform_;
        out[2] = std::make_tuple(x_p2, spent+1);
        return std::make_tuple(out, 3);
    }
    else
    {
        const double deform = deformations_.at(feature_id);
        if (deform == 0.0)
        {
            return std::make_tuple(out, 1);
        }
        Row<NX> x_p1 = x;
        x_p1[feature_id] += deform;
        out[1] = std::make_tuple(x_p1, spent+1);
        Row<NX> x_p2 = x;
        x_p2[feature_id] -= deform;
        out[2] = std::make_tuple(x_p2, spent+1);
        return std::make_tuple(out, 3);
    }
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
}

template<size_t NX>
void Attacker<NX>::remove_useless_rules()
{
    auto it = std::remove_if(rules_.begin(), rules_.end(), [&](auto& rule){
        return !features_.contains(rule.get_target_feature());
    });
    rules_.resize(std::distance(rules_.begin(), it));
}
