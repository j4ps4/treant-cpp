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
    case AttackType::Flat:
    {
        Util::die("called compute_attack with Flat attack!!");
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
    // const auto H = X.rows();
    // for (int64_t i = 0; i < H; i++)
    // {
    //     const Row<N>& rw = X.row(i);
    //     for (auto j : features_)
    //     {
    //         auto key = std::make_tuple(rw, j);
    //         //compute_attack(rw, j, 0);
    //         attacks_[key] = compute_attack(rw, j, 0);
    //     }
    // }
}

// template<typename... Ts>
// void Attacker<Ts...>::load_attacks(const std::filesystem::path& fn)
// {
//     std::ifstream is(fn, std::ios::binary);
//     cereal::BinaryInputArchive archive(is);
//     AttackDict<Ts...> attacks;
//     archive(attacks);
//     attacks_ = std::move(attacks);
//     Util::info("loaded {} attacks.\n", attacks_.size());
// }

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
TupleVec<NX> Attacker<NX>::attack(const Row<NX>& x, size_t feature_id, int cost) const
{
    if (is_flat_)
    {
        TupleVec<NX> out;
        out.reserve(1 + 2 * (budget_ - cost));
        out.push_back(std::make_tuple(x, cost));
        int new_cost = cost + 1;
        int multipl = 1;
        while (new_cost <= budget_)
        {
            Row<NX> x_p1 = x;
            x_p1[feature_id] += multipl * flat_deform_;
            out.push_back(std::make_tuple(x_p1, new_cost));
            Row<NX> x_p2 = x;
            x_p2[feature_id] -= multipl * flat_deform_;
            out.push_back(std::make_tuple(x_p2, new_cost));
            new_cost++;
            multipl++;
        }
        return out;
    }
    else if (features_.contains(feature_id))
    {
        // auto attack_key = std::make_tuple(x, feature_id);
        // if (!attacks_.contains(attack_key))
        //     attacks_[attack_key] = compute_attack(x, feature_id, 0);

        // auto& attacks_xf = attacks_.at(attack_key);
        auto attacks_xf = compute_attack(x, feature_id, 0);
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
TupleVec<NX> Attacker<NX>::single_attack(const Row<NX>& x, size_t feature_id, int budget) const
{
    TupleVec<NX> out;
    if (is_flat_)
    {
        if (budget <= 0)
            return out;
        Row<NX> x_p1 = x;
        x_p1[feature_id] += flat_deform_;
        out.push_back(std::make_tuple(x_p1, budget-1));
        Row<NX> x_p2 = x;
        x_p2[feature_id] -= flat_deform_;
        out.push_back(std::make_tuple(x_p2, budget-1));
        return out;
    }
    else
    {
    auto rule_it = rules_.cbegin();
    while (true)
    {
        rule_it = std::find_if(rule_it, rules_.cend(), [feature_id](const auto& rule){
            return rule.get_target_feature() == feature_id;
        });
        if (rule_it != rules_.cend())
        {
            int cost = rule_it->get_cost();
            if (cost > budget)
            {
                rule_it++;
                continue;
            }
            if (!rule_it->template is_applicable<NX>(x))
            {
                rule_it++;
                continue;
            }
            auto att = rule_it->template apply<NX>(x);
            auto tup = std::make_tuple(att, budget-cost);
            out.push_back(tup);
            rule_it++;
        }
        else
        {
            return out;
        }
    }
    }
}
