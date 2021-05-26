#include <deque>
#include <algorithm>
#include <iterator>
#include <array>

#include <fmt/core.h>
//#include "DF2/DF_util.h"

const char* BUDG = "_budget";

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

template<typename... Ts>
TupleVec<Ts...> Attacker<Ts...>::compute_attack(const std::tuple<Ts...>& x, size_t feature_id, int cost) const
{
    using namespace Util;
    std::deque<PairT<Ts...>> queue = {{x, cost}};
    TupleVec<Ts...> attacks;

    while (queue.size() > 0)
    {
        auto [inst, budg] = queue.back();
        queue.pop_back();
        attacks.push_back(std::make_pair(inst, budg));
        AttkList applicables1;
        std::copy_if(rules_.cbegin(), rules_.cend(), std::back_inserter(applicables1),
            [=](const auto& atk){return atk.get_target_feature() == feature_id;});
        AttkList applicables2;
        std::copy_if(applicables1.cbegin(), applicables1.cend(), std::back_inserter(applicables2),
            [&](const auto& atk){return atk.is_applicable(inst);});
        for (auto& r : applicables2)
        {
            if (budget_ >= budg + r.get_cost())
            {
                auto x_prime = r.apply(inst);
                auto cost_prime = budg + r.get_cost();
                auto pair = std::make_pair(x_prime, cost_prime);
                if (!check_equal_perturbation(attacks, pair))
                {
                    queue.push_front(pair);
                }
                auto f = r.get_target_feature();
                std::array<double,2> sarr = {tuple_index<double>(inst,f), tuple_index<double>(x_prime,f)};
                std::sort(sarr.begin(), sarr.end());
                auto [low, high] = sarr;
                auto [pre1, pre2] = r.get_pre_interval();
                std::vector<double> z;
                if (low < pre1 && pre1 < high)
                    z.push_back(pre1);
                if (low < pre2 && pre2 < high)
                    z.push_back(pre2);
                // for (double zi : z)
                // {
                //     x_prime = inst;
                //     std::get<f>(x_prime) = zi;
                //     auto pair = std::make_pair(x_prime, cost_prime);
                //     if (!check_equal_perturbation(attacks, pair))
                //         queue.push_front(pair);
                // }
            }
        }
    }
    return attacks;
}

template<typename... Ts>
void Attacker<Ts...>::compute_attacks(const DF<Ts...>& X, const std::string& attacks_fn)
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
            auto key = std::make_pair(i, j);
            //compute_attack(rw, j, 0);
            attacks_[key] = compute_attack(rw, j, 0);
        }
    }
    fmt::print("computed {} attacks.\n", attacks_.size());
    fmt::print("first 10 attacks:\n");
    size_t count = 0;
    for (auto& [key, vect] : attacks_)
    {
        if (count >= 10)
            break;
        for (auto& pair : vect)
        {
            auto& vec = std::get<0>(pair);
            auto cost = std::get<1>(pair);
            std::cout << vec << ", cost: " << cost << "\n";
        }
        count++;
    }
    // dump attacks_ to file
}