#include <deque>
#include <algorithm>
#include <iterator>

#include "Attacker.h"

const char* BUDG = "_budget";

bool is_equal_perturbation(const DFRView& x, const std::pair<DFR, int>& y)
{
    return x == std::get<0>(y) && x.get<int>(BUDG) <= std::get<1>(y);
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
                bool found = false;
                for (size_t i = 0; i < height; i++)
                {
                    auto row = attacks.get_row_view(i);
                    if (is_equal_perturbation(row, pair))
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    queue.push_front(pair);
            }
        }
    }
}