#pragma once

#include <unordered_map>
#include <vector>
#include <tuple>
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

template<typename... Ts>
using PairT = std::tuple<std::tuple<Ts...>, int>;
template<typename... Ts>
using TupleVec = std::vector<std::tuple<std::tuple<Ts...>, int>>;
template<typename... Ts>
using AttackDict = std::unordered_map<std::tuple<size_t,size_t>, TupleVec<Ts...>, hash<size_t,size_t>>;

template<typename... Ts>
class Attacker
{
    // TODO: unordered map
    // (row_idx, feature_id) -> DF
public:
    Attacker(AttkList&& rules, int budget) :
        budget_(budget), rules_(std::forward<AttkList>(rules)) {}
    bool is_filled() const;

    template<typename... AF, size_t... Is>
    void compute_attacks(const DF<Ts...>& X, const std::filesystem::path& attacks_fn, std::index_sequence<Is...>);
    
    template<typename... Fs>
    void print_rules() const;

    void load_attacks(const std::filesystem::path& fn);
private:
    template<typename... AF, size_t... Is>
    TupleVec<Ts...> compute_attack(const std::tuple<Ts...>& x, size_t feature_id, int cost, std::index_sequence<Is...>) const;
    int budget_;
    AttkList rules_;

    AttackDict<Ts...> attacks_;
};

#include "Attacker.tpp"
