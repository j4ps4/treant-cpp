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
template<size_t N, size_t L>
using TupleArr = std::array<PairT<N>, L>;
using CostArr = std::array<int, 2>;
// template<size_t N>
// using AttackDict = std::unordered_map<std::tuple<Row<N>,size_t>, TupleVec<N>, row_hash<N>, key_equal<N>>;

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
            compute_deformations();
        }
    
    Attacker() = default;

    Attacker(const Attacker<NX>& other) = default;
    Attacker(Attacker<NX>&& other) = default;
    Attacker& operator=(Attacker<NX>&& other) = default;
    Attacker& operator=(const Attacker<NX>& other) = default;

    bool is_filled() const;

    void print_rules() const;

    const std::set<size_t>& target_features() const {return features_;}

    //void load_attacks(const std::filesystem::path& fn);

    int get_budget() const noexcept {return budget_;}

    void set_budget(int budget) noexcept {budget_ = budget;}

    void set_feats(const std::set<size_t>& feats) 
    {
        features_ = feats; 
        remove_useless_rules();
        compute_deformations();
    }

    bool is_constant() const noexcept {return is_constant_;}

    bool is_normal() const noexcept {return type_ == AttackType::Normal;}

    double get_deformation() const noexcept {return flat_deform_;}

    // returns the costs necessary to push instance to left and right of feature value
    // (cost = -1 means not possible)
    std::tuple<CostArr,CostArr> push_costs(const Row<NX>& x, size_t feature_id, double split_value, int spent) const;

    TupleArr<NX, 2> adjacent_attack(const Row<NX>& x, size_t feature_id, int spent) const;

    // return the deformation for a feature
    double get_deformation(size_t feature_id) const 
    {
        if (deformations_.empty())
        {
            Util::die("no deformations -- empty attacker?");
        }
        return deformations_.at(feature_id);
    }

    template<typename Archive>
    void save(Archive& archive) const
    {
        archive(budget_, type_, rules_, features_, is_constant_, flat_deform_);
    }

    template<typename Archive>
    void load(Archive& archive)
    {
        archive(budget_, type_, rules_, features_, is_constant_, flat_deform_);
        compute_deformations();
    }

private:
    void remove_useless_rules();
    void compute_target_features();
    void compute_deformations();
    int budget_;
    AttackType type_;
    AttkList rules_;
    std::set<size_t> features_; // features which are targeted by rules
    bool is_constant_ = false;
    double flat_deform_;
    std::unordered_map<size_t, double> deformations_;
    //AttackDict<NX> attacks_;
};

#include "Attacker.tpp"
