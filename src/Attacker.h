#pragma once

#include <unordered_map>
#include <vector>
#include <tuple>
#include <array>
#include <filesystem>

#include "DF2/DF_def.h"
#include "AttackerRule.h"

template<typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//  taken from https://stackoverflow.com/a/7222201/916549
//
// template<typename S, typename T>
// struct hash
// {
//     inline size_t operator()(const std::tuple<S, T>& val) const
//     {
//         size_t seed = 0;
//         hash_combine(seed, std::get<0>(val));
//         hash_combine(seed, std::get<1>(val));
//         return seed;
//     }
// };

template<size_t N>
struct row_hash
{
    inline size_t operator()(const std::tuple<Row<N>,size_t>& val) const
    {
        const auto& row = std::get<0>(val);
        const auto& fid = std::get<1>(val);
        size_t seed = 0;
        for (size_t i = 0; i < N; i++)
            hash_combine(seed, row(i));
        hash_combine(seed, fid);
        return seed;
    }
};

template<size_t N>
struct key_equal
{
    constexpr bool operator()( const std::tuple<Row<N>,size_t>& lhs,
                     const std::tuple<Row<N>,size_t>& rhs ) const
    {
        const auto& row1 = std::get<0>(lhs);
        const auto& row2 = std::get<0>(rhs);
        const auto& fid1 = std::get<1>(lhs);
        const auto& fid2 = std::get<1>(rhs);
        return (row1 == row2).all() && fid1 == fid2; 
    }
};

template<size_t N>
using PairT = std::tuple<Row<N>, int>;
template<size_t N>
using TupleVec = std::vector<PairT<N>>;
template<size_t N>
using AttackDict = std::unordered_map<std::tuple<Row<N>,size_t>, TupleVec<N>, row_hash<N>, key_equal<N>>;

template<size_t NX>
class Attacker
{
public:
    Attacker(AttkList&& rules, int budget) :
        budget_(budget), rules_(std::forward<AttkList>(rules)) {compute_target_features();}
    
    Attacker() = default;

    bool is_filled() const;

    void compute_attacks(const DF<NX>& X, const std::filesystem::path& attacks_fn);
    
    void print_rules() const;

    //void load_attacks(const std::filesystem::path& fn);

    int get_budget() const noexcept {return budget_;}

    TupleVec<NX> attack(const Row<NX>& x, size_t feature_id, int cost);

private:
    void compute_target_features();
    TupleVec<NX> compute_attack(const Row<NX>& rw, size_t feature_id, int cost) const;
    int budget_;
    AttkList rules_;
    std::vector<size_t> features_; // features which are targeted by rules

    AttackDict<NX> attacks_;
};

#include "Attacker.tpp"
