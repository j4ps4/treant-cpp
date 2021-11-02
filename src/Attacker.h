#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <tuple>
#include <array>
#include <filesystem>

#include <cereal/types/set.hpp>
#include <cereal/types/list.hpp>

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
    std::set<size_t> gen_feature_set()
    {
        auto gen = [](){static size_t x = 0; return x++;};
        std::set<size_t> out;
        std::generate_n(std::inserter(out, out.begin()), NX, gen);
        return out;
    }

public:
    Attacker(LoadType&& rules, int budget, const std::set<size_t>& blacklist) :
        budget_(budget)
        {
            auto& rul = std::get<0>(rules);
            rules_ = std::move(rul);
            type_ = std::get<1>(rules);
            if (type_ == AttackType::Constant)
            {
                is_constant_ = true;
                flat_deform_ = rules_.front().get_post();
                const static std::set<size_t> all_features = gen_feature_set();
                std::set_difference(all_features.begin(), all_features.end(),
                    blacklist.begin(), blacklist.end(),
                    std::inserter(features_, features_.begin()));
            }
            else
            {
                is_constant_ = false;
                compute_target_features();
            }
        }
    
    Attacker() = default;

    Attacker(const Attacker<NX>& other) = default;

    bool is_filled() const;

    void compute_attacks(const DF<NX>& X) const;
    
    void print_rules() const;

    const std::set<size_t>& target_features() const {return features_;}

    //void load_attacks(const std::filesystem::path& fn);

    int get_budget() const noexcept {return budget_;}

    void set_budget(int budget) noexcept {budget_ = budget;}

    bool is_constant() const noexcept {return is_constant_;}

    bool is_normal() const noexcept {return type_ == AttackType::Normal;}

    double get_deformation() const noexcept {return flat_deform_;}

    // return only the maximum attack
    TupleVec<NX> max_attack(const Row<NX>& x, size_t feature_id) const;

    // returns first attacks for a given instance, spent is the amount spent for this instance
    TupleVec<NX> single_attack(const Row<NX>& x, size_t feature_id, int spent, bool keep_orig) const;

    template<typename Archive>
    void save(Archive& archive) const
    {
        archive(budget_, type_, rules_, features_, is_constant_, flat_deform_);
    }

    template<typename Archive>
    void load(Archive& archive)
    {
        archive(budget_, type_, rules_, features_, is_constant_, flat_deform_);
    }

private:
    void compute_target_features();
    TupleVec<NX> compute_attack(const Row<NX>& rw, size_t feature_id, int cost) const;
    int budget_;
    AttackType type_;
    AttkList rules_;
    std::set<size_t> features_; // features which are targeted by rules
    bool is_constant_;
    double flat_deform_;
    //AttackDict<NX> attacks_;
};

#include "Attacker.tpp"
