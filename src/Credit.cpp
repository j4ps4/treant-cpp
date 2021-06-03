#include <filesystem>

#include "Credit.h"
#include "AttackerRule.h"
#include "DF2/DF_read.h"

#define ATTACK_TYPES int8_t,double,int8_t,double
//#define ATTACK_TYPES double,double,double,double

using FEATURE_ID = std::index_sequence<5,11,6,0>;

namespace credit
{

const std::filesystem::path data_dir = "data/credit/";
const std::filesystem::path train_file = data_dir / "train.csv.bz2";
const std::filesystem::path valid_file = data_dir / "valid.csv.bz2";
const std::filesystem::path test_file = data_dir / "test.csv.bz2";
const std::filesystem::path json_file = data_dir / "attacks.json";
const std::filesystem::path attack_file = data_dir / "credit.cereal";

DataFrame::DataFrame(DF<CREDIT_DATATYPES>&& df) :
    df_(std::move(df)) {}

DataFrame::DataFrame(size_t cap) :
    df_(cap) {}

DataFrame append(const DataFrame& lhs, const DataFrame& rhs)
{
    DataFrame out(lhs.df_.height() +  rhs.df_.height());
    for (const auto& row : lhs.df_)
        out.df_.append_row(row);
    for (const auto& row : rhs.df_)
        out.df_.append_row(row);
    return out;
}

cpp::result<DataFrame,std::string> read_bz2()
{
    auto res = df::read_bz2<CREDIT_DATATYPES>(train_file.c_str()).flat_map([&](const auto& train){
        return df::read_bz2<CREDIT_DATATYPES>(valid_file.c_str()).flat_map([&](const auto& valid){
            return df::read_bz2<CREDIT_DATATYPES>(test_file.c_str()).map([&](const auto& test){
                return append3(train, valid, test);
            });
        });
    });
    if (res.has_error())
        return cpp::failure(res.error());
    return DataFrame(std::move(res.value()));
}

Hostile::Hostile(Attacker<CREDIT_DATATYPES>&& atkr) :
    atkr_(std::move(atkr)) {}

cpp::result<Hostile,std::string> new_Hostile(int budget)
{
    auto res = load_attack_rules<ATTACK_TYPES>(json_file);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    return Attacker<CREDIT_DATATYPES>(std::move(rulz), budget);
}

void Hostile::attack_dataset(const DataFrame& X, ForceCompute force)
{
    if (force == ForceCompute::No && std::filesystem::exists(attack_file))
    {
        Util::info("loading attacked dataset from {}...", attack_file.c_str());
        atkr_.load_attacks(attack_file);
    }
    else
    {
        Util::info("computing attacks and storing into {}...", attack_file.c_str());
        atkr_.compute_attacks<ATTACK_TYPES>(X.df_, attack_file, FEATURE_ID{});
    }
}

void Hostile::dump_attack_rules() const
{
    atkr_.print_rules<ATTACK_TYPES>();
}

}