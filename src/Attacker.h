#pragma once

#include <unordered_map>
#include <vector>
#include <tuple>
#include <array>
#include <filesystem>

#include "DF2/DF.h"
#include "AttackerRule.h"

template<typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//  taken from https://stackoverflow.com/a/7222201/916549
//
template<typename S, typename T>
struct hash
{
    inline size_t operator()(const std::tuple<S, T>& val) const
    {
        size_t seed = 0;
        hash_combine(seed, std::get<0>(val));
        hash_combine(seed, std::get<1>(val));
        return seed;
    }
};

template<size_t N>
using PairT = std::tuple<std::array<double, N>, int>;
template<size_t N>
using TupleVec = std::vector<std::tuple<std::array<double, N>, int>>;
template<size_t N>
using AttackDict = std::unordered_map<std::tuple<size_t,size_t>, TupleVec<N>, hash<size_t,size_t>>;

template<size_t N>
class Attacker
{
public:
    Attacker(AttkList&& rules, int budget) :
        budget_(budget), rules_(std::forward<AttkList>(rules)) {}
    bool is_filled() const;

    void compute_attacks(const DF<N>& X, const std::filesystem::path& attacks_fn);
    
    template<typename... Fs>
    void print_rules() const;

    void load_attacks(const std::filesystem::path& fn);

    int get_budget() const noexcept {return budget_;}
private:
    TupleVec<N> compute_attack(const std::array<double, N>& x, size_t feature_id, int cost) const;
    int budget_;
    AttkList rules_;

    AttackDict<N> attacks_;
};

#include "Attacker.tpp"
