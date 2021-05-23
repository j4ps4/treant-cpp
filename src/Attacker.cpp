#include <deque>
#include <algorithm>
#include <iterator>
#include <array>
#include <vector>

#include "Attacker.h"

const char* BUDG = "_budget";

bool is_equal_perturbation(const DFRView& x, const std::pair<DFR, int32_t>& y)
{
    return x.equal_disregarding(std::get<0>(y), BUDG) &&
         x.get<int32_t>(BUDG) <= std::get<1>(y);
}

bool check_equal_perturbation(const DF& attacks, const std::pair<DFR, int32_t> y)
{
    auto height = attacks.shape().first;
    for (size_t i = 0; i < height; i++)
    {
        auto row = attacks.get_row_view(i);
        if (is_equal_perturbation(row, pair))
        {
            return true;
        }
    }
    return false;
}

DF Attacker::compute_attack(const DFRView& x, size_t feature_id, int cost) const
{
    std::deque<std::pair<DFR, int>> queue = {{x.copy(), cost}};
    DF attacks;
    attacks.set_colmap(x.get_colmap());
    attacks.add_column(BUDG, DataType::Int);
    while (queue.size() > 0)
    {
        auto [inst, budg] = queue.back();
        queue.pop_back();
        attacks.append_row(inst);
        attacks.append_value(BUDG, budg);
        AttkList applicables1;
        std::copy_if(rules_.cbegin(), rules_.cend(), std::back_inserter(applicables1),
            [=](const auto& atk){return atk.get_target_feature() == feature_id;});
        AttkList applicables2;
        std::copy_if(applicables1.cbegin(), applicables1.cend(), std::back_inserter(applicables2),
            [&](const auto& atk){return atk.is_applicable(x);});
        for (auto& r : applicables2)
        {
            if (budget_ >= budg + r.get_cost())
            {
                auto x_prime = r.apply(x);
                auto cost_prime = budg + r.get_cost();
                auto pair = std::make_pair(x_prime, cost_prime);
                auto height = attacks.shape().first;
                if (!check_equal_perturbation(attacks, pair))
                    queue.push_front(pair);
                auto f = r.get_target_feature();
                std::array<double,2> sarr = {x.get_as_f64(f), x_prime.get_as_f64(f)};
                std::sort(sarr.begin(), sarr.end());
                auto [low, high] = sarr;
                auto [pre1, pre2] = r.get_pre_interval();
                std::vector<double> z;
                if (low < pre1 && pre1 < high)
                    z.push_back(pre1);
                if (low < pre2 && pre2 < high)
                    z.push_back(pre2);
                for (double zi : z)
                {
                    x_prime = x.copy();
                    x_prime.assign_value(f, zi);
                    auto pair = std::make_pair(x_prime, cost_prime);
                    if (!check_equal_perturbation(attacks, pair))
                        queue.push_front(pair);
                }
            }
        }
    }
    return attacks;
}