#pragma once

#include <map>
#include <vector>
#include <tuple>

#include "DF2/DF.h"
#include "AttackerRule.h"

template<typename... Ts>
using PairT = std::pair<std::tuple<Ts...>, int>;
template<typename... Ts>
using TupleVec = std::vector<std::pair<std::tuple<Ts...>, int>>;
template<typename... Ts>
using AttackDict = std::map<std::pair<size_t,size_t>, TupleVec<Ts...>>;

template<typename... Ts>
class Attacker
{
    // TODO: unordered map
    // (row_idx, feature_id) -> DF
public:
    Attacker(const AttkList& rules, int budget) :
        budget_(budget), rules_(rules) {}
    bool is_filled() const;
    void compute_attacks(const DF<Ts...>& X, const std::string& attacks_fn);
private:
    TupleVec<Ts...> compute_attack(const std::tuple<Ts...>& x, size_t feature_id, int cost) const;
    int budget_;
    AttkList rules_;

    AttackDict<Ts...> attacks_;
};

#include "Attacker.tpp"