#pragma once

#include <map>

#include "DF.h"
#include "AttackerRule.h"

class Attacker
{
    // TODO: unordered map
    // (row_idx, feature_id) -> DF
    using AttackDict = std::map<std::pair<size_t,size_t>, DF>;
public:
    Attacker(const AttkList& rules, int budget) :
        budget_(budget), rules_(rules) {}
    bool is_filled() const;
    void compute_attacks(const DF& X) const;
private:
    DF compute_attack(const DFRView& x, size_t feature_id, int cost) const;
    int budget_;
    AttkList rules_;
    AttackDict attacks_;
};