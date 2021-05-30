#include <deque>
#include <algorithm>
#include <iterator>
#include <array>

#include <fmt/core.h>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

#include "util.h"

namespace 
{

template<typename... Ts>
bool is_equal_perturbation(const PairT<Ts...>& x, const PairT<Ts...>& y)
{
    return std::get<0>(x) == std::get<0>(y) &&
         std::get<1>(x) <= std::get<1>(y);
}

template<typename... Ts>
bool check_equal_perturbation(const TupleVec<Ts...>& attacks, const PairT<Ts...>& y)
{
    for (size_t i = 0; i < attacks.size(); i++)
    {
        auto row = attacks[i];
        if (is_equal_perturbation(row, y))
        {
            return true;
        }
    }
    return false;
}

template<typename T>
struct head_impl;

template<std::size_t I, std::size_t... Is>
struct head_impl<std::index_sequence<I, Is...>>
{
    static constexpr size_t value = I;
};

template<typename T>
struct tail_impl;

template<std::size_t I, std::size_t... Is>
struct tail_impl<std::index_sequence<I, Is...>>
{
    using value = std::index_sequence<Is...>;
};

template<typename T>
using tail = typename tail_impl<T>::value;

template<typename T>
constexpr size_t head()
{
    return head_impl<T>::value;
}

template<typename... Ts>
struct AttackComputer
{

template<typename F, size_t I>
static void comp_impl(const TupleVec<Ts...>& attacks, AttkList::const_iterator& rules_it, std::deque<PairT<Ts...>>& queue,
    const std::tuple<Ts...>& inst, int total_budget, int budget, size_t feature_id, int cost, std::index_sequence<I>)
{
    constexpr size_t H = head<std::index_sequence<I>>();
    const auto& rule = *rules_it;
    if (rule.get_target_feature() == feature_id && rule.is_applicable<H,F>(inst))
    {
        if (total_budget >= budget + rule.get_cost())
        {
            auto x_prime = rule.apply<H,F>(inst);
            auto cost_prime = budget + rule.get_cost();
            auto pair = std::make_tuple(x_prime, cost_prime);
            if (!check_equal_perturbation(attacks, pair))
            {
                queue.push_front(pair);
            }
        }
    }
}

template<typename F1, typename F2, typename... Fs, size_t... Is>
static void comp_impl(const TupleVec<Ts...>& attacks, AttkList::const_iterator& rules_it, std::deque<PairT<Ts...>>& queue,
    const std::tuple<Ts...>& inst, int total_budget, int budget, size_t feature_id, int cost, std::index_sequence<Is...>)
{
    static_assert(sizeof...(Is) == sizeof...(Fs)+2, "size mismatch");
    constexpr size_t H = head<std::index_sequence<Is...>>();
    const auto& rule = *rules_it;
    if (rule.get_target_feature() == feature_id && rule.is_applicable<H,F1>(inst))
    {
        if (total_budget >= budget + rule.get_cost())
        {
            auto x_prime = rule.apply<H,F1>(inst);
            auto cost_prime = budget + rule.get_cost();
            auto pair = std::make_tuple(x_prime, cost_prime);
            if (!check_equal_perturbation(attacks, pair))
            {
                queue.push_front(pair);
            }
        }
    }
    rules_it++;
    comp_impl<F2,Fs...>(attacks, rules_it, queue, inst, total_budget, budget, feature_id, cost, tail<std::index_sequence<Is...>>{});
}

};

}

template<typename... Ts>
template<typename... AF, size_t... Is>
TupleVec<Ts...> Attacker<Ts...>::compute_attack(const std::tuple<Ts...>& x, size_t feature_id, int cost, std::index_sequence<Is...>) const
{
    using namespace Util;
    std::deque<PairT<Ts...>> queue = {{x, cost}};
    TupleVec<Ts...> attacks;

    while (queue.size() > 0)
    {
        auto [inst, budget] = queue.back();
        queue.pop_back();
        attacks.push_back(std::make_tuple(inst, budget));
        auto rules_it = rules_.cbegin();
        AttackComputer<Ts...>::template comp_impl<AF...>(attacks, rules_it, queue, inst, budget_, budget, feature_id, cost, std::index_sequence<Is...>{});
        // auto applicables = rules_ | ranges::views::filter([=](const auto& atk){return atk.get_target_feature() == feature_id;})
		// 		| ranges::views::filter([&](const auto& atk){return atk.is_applicable(inst);});
        // for (auto& r : applicables)
        // {
        //     if (budget_ >= budg + r.get_cost())
        //     {
        //         auto x_prime = r.apply(inst);
        //         auto cost_prime = budg + r.get_cost();
        //         auto pair = std::make_pair(x_prime, cost_prime);
        //         if (!check_equal_perturbation(attacks, pair))
        //         {
        //             queue.push_front(pair);
        //         }
                // auto f = r.get_target_feature();
                // std::array<double,2> sarr = {tuple_index<double>(inst,f), tuple_index<double>(x_prime,f)};
                // std::sort(sarr.begin(), sarr.end());
                // auto [low, high] = sarr;
                // auto [pre1, pre2] = r.get_pre_interval();
                // std::vector<double> z;
                // if (low < pre1 && pre1 < high)
                //     z.push_back(pre1);
                // if (low < pre2 && pre2 < high)
                //     z.push_back(pre2);
                // for (double zi : z)
                // {
                //     x_prime = inst;
                //     std::get<f>(x_prime) = zi;
                //     auto pair = std::make_pair(x_prime, cost_prime);
                //     if (!check_equal_perturbation(attacks, pair))
                //         queue.push_front(pair);
                // }
        //     }
        // }
    }
    return attacks;
}

template<typename... Ts>
template<typename... AF, size_t... Is>
void Attacker<Ts...>::compute_attacks(const DF<Ts...>& X, const std::filesystem::path& attacks_fn, std::index_sequence<Is...>)
{
    std::vector<size_t> fs;
    for (const auto& r : rules_)
    {
        auto f = r.get_target_feature();
        fs.push_back(f);
    }
    auto H = X.height();
    for (size_t i = 0; i < H; i++)
    {
        auto rw = X[i];
        for (auto j : fs)
        {
            auto key = std::make_tuple(i, j);
            //compute_attack(rw, j, 0);
            attacks_[key] = compute_attack<AF...>(rw, j, 0, std::index_sequence<Is...>{});
        }
    }
    Util::info("computed {} attacks.\n", attacks_.size());
    // fmt::print("first 10 attacks:\n");
    // size_t count = 0;
    // for (auto& [key, vect] : attacks_)
    // {
    //     if (count >= 10)
    //         break;
    //     for (auto& pair : vect)
    //     {
    //         auto& vec = std::get<0>(pair);
    //         auto cost = std::get<1>(pair);
    //         fmt::print("{}, cost: {}\n", vec, cost);
    //     }
    //     count++;
    // }
    // dump attacks_ to file
    std::ofstream os(attacks_fn, std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    archive(attacks_);
}

template<typename... Ts>
void Attacker<Ts...>::load_attacks(const std::filesystem::path& fn)
{
    std::ifstream is(fn, std::ios::binary);
    cereal::BinaryInputArchive archive(is);
    AttackDict<Ts...> attacks;
    archive(attacks);
    attacks_ = std::move(attacks);
    Util::info("loaded {} attacks.\n", attacks_.size());
}

namespace
{

template<typename F>
void print_helper(AttkList::const_iterator& r)
{
    fmt::print("{}\n", r->debug_str<F>());
}

template<typename F1, typename F2, typename... Fs>
void print_helper(AttkList::const_iterator& r)
{
    fmt::print("{}\n", r->debug_str<F1>());
    print_helper<F2,Fs...>(++r);
}

}

template<typename... Ts>
template<typename... Fs>
void Attacker<Ts...>::print_rules() const
{
    auto it = rules_.cbegin();
    print_helper<Fs...>(it);
}