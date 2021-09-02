#include <deque>
#include <algorithm>
#include <iterator>
#include <array>
#include <iostream>

#include <fmt/core.h>
#include <fstream>

#include "util.h"

namespace 
{

template<size_t N>
bool is_equal_perturbation(const PairT<N>& x, const PairT<N>& y)
{
    return (std::get<0>(x) == std::get<0>(y)).all() &&
         std::get<1>(x) <= std::get<1>(y);
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
void compute_impl(const TupleVec<N>& attacks, const AttkList& rules, std::deque<PairT<N>>& queue,
    const Row<N>& inst, int total_budget, int budget, size_t feature_id)
{
    for (const auto& rule : rules)
    {
    if (rule.get_target_feature() == feature_id && rule.is_applicable<N>(inst))
    {
        if (total_budget >= budget + rule.get_cost())
        {
            auto x_prime = rule.apply<N>(inst);
            auto cost_prime = budget + rule.get_cost();
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



}

template<size_t N>
TupleVec<N> Attacker<N>::compute_attack(const Row<N>& rw, size_t feature_id, int cost) const
{
    using namespace Util;
    std::deque<PairT<N>> queue = {{rw, cost}};
    TupleVec<N> attacks;

    while (queue.size() > 0)
    {
        auto [inst, budget] = queue.back();
        queue.pop_back();
        attacks.push_back(std::make_tuple(inst, budget));
        compute_impl<N>(attacks, rules_, queue, inst, budget_, budget, feature_id);
    }
    return attacks;
}

template<size_t N>
void Attacker<N>::compute_attacks(const DF<N>& X)
{
    const auto H = X.rows();
    for (int64_t i = 0; i < H; i++)
    {
        const Row<N>& rw = X.row(i);
        for (auto j : features_)
        {
            auto key = std::make_tuple(rw, j);
            //compute_attack(rw, j, 0);
            attacks_[key] = compute_attack(rw, j, 0);
        }
    }
}

template<size_t N>
void Attacker<N>::print_rules() const
{
    for (const auto& r : rules_)
        fmt::print("{}\n", r.debug_str());
}

template<size_t NX>
void Attacker<NX>::compute_target_features()
{
    for (const auto& r : rules_)
    {
        auto f = r.get_target_feature();
        features_.push_back(f);
    }
}

template<size_t NX>
TupleVec<NX> Attacker<NX>::attack(const Row<NX>& x, size_t feature_id, int cost)
{

    if (std::find(features_.begin(), features_.end(), feature_id) != features_.end())
    {
        auto attack_key = std::make_tuple(x, feature_id);
        if (!attacks_.contains(attack_key))
            attacks_[attack_key] = compute_attack(x, feature_id, cost);

        auto& attacks_xf = attacks_.at(attack_key);
        TupleVec<NX> out;
        std::copy_if(attacks_xf.begin(), attacks_xf.end(), std::back_inserter(out), [this, cost](auto& pair){
            return std::get<1>(pair) <= budget_ - cost;
        });
        for (auto& pair : out)
            std::get<1>(pair) += cost;
        return out;
    }
    else
    {
        TupleVec<NX> out{{x, cost}};
        return out;
    }

}

template<size_t NX>
std::optional<TupleVec<NX>> Attacker<NX>::maybe_attack(const Row<NX>& x, size_t feature_id, int cost)
{
    if (std::find(features_.begin(), features_.end(), feature_id) != features_.end())
    {
        auto attack_key = std::make_tuple(x, feature_id);
        if (!attacks_.contains(attack_key))
            attacks_[attack_key] = compute_attack(x, feature_id, cost);

        auto& attacks_xf = attacks_.at(attack_key);
        TupleVec<NX> out;
        // first attack is the original instance, ignore it
        std::copy_if(attacks_xf.begin()+1, attacks_xf.end(), std::back_inserter(out), [this, cost](auto& pair){
            return std::get<1>(pair) <= budget_ - cost;
        });
        if (out.empty())
            return {};
        for (auto& pair : out)
            std::get<1>(pair) += cost;
        return out;
    }
    else
    {
        return {};
    }

}