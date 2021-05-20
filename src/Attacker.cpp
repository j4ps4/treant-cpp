#include <vector>
#include <algorithm>
#include <iterator>

#include "Attacker.h"

DF Attacker::compute_attack(const DFRView& x, size_t feature_id, int cost) const
{
    std::vector<std::pair<DFRView, int>> queue = {{x, cost}};
    DF out;
    out.set_colmap(x.get_colmap());
    out.add_column("budget", DataType::Int);
    while (queue.size() > 0)
    {
        auto [inst, budg] = queue.back();
        queue.pop_back();
        out.append_row(inst);
        out.append_value("budget", budg);
        AttkList applicables1;
        std::copy_if(rules_.cbegin(), rules_.cend(), std::back_inserter(applicables1),
            [=](const auto& atk){return atk.get_target_feature() == feature_id;});
        AttkList applicables2;
        std::copy_if(applicables1.cbegin(), applicables1.cend(), std::back_inserter(applicables2),
            [&](const auto& atk){return atk.is_applicable(x);});
    }
}