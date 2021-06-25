#include <filesystem>

#include "Credit.h"
#include "AttackerRule.h"
#include "DF2/DF_read.h"

namespace credit
{

const std::filesystem::path data_dir = "data/credit/";
const std::filesystem::path train_file = data_dir / "train.csv.bz2";
const std::filesystem::path valid_file = data_dir / "valid.csv.bz2";
const std::filesystem::path test_file = data_dir / "test.csv.bz2";
const std::filesystem::path json_file = data_dir / "attacks.json";

// std::filesystem::path Hostile::attack_filename() const
// {
//     auto format = fmt::format("credit_B{}.atks", atkr_.get_budget());
//     return data_dir / format;
// }

template<size_t N>
DF<N> append3(const DF<N>& fst, const DF<N>& snd, const DF<N>& thrd)
{
    auto total_rows = fst.rows() + snd.rows() + thrd.rows();
    DF<N> out = Eigen::ArrayXXd::Zero(total_rows,N);
    size_t out_idx = 0;
    for (int64_t i = 0; i < fst.rows(); i++)
        out.row(out_idx++) = fst.row(i);
    for (int64_t i = 0; i < snd.rows(); i++)
        out.row(out_idx++) = snd.row(i);
    for (int64_t i = 0; i < thrd.rows(); i++)
        out.row(out_idx++) = thrd.row(i);
    return out;
}

cpp::result<DF<CREDIT_SIZE>,std::string> read_bz2()
{
    auto res = df::read_bz2<CREDIT_SIZE>(train_file.c_str()).flat_map([&](const auto& train){
        return df::read_bz2<CREDIT_SIZE>(valid_file.c_str()).flat_map([&](const auto& valid){
            return df::read_bz2<CREDIT_SIZE>(test_file.c_str()).map([&](const auto& test){
                return append3<CREDIT_SIZE>(train, valid, test);
            });
        });
    });
    return res;
    // if (res.has_error())
    //     return cpp::failure(res.error());
    // return res.value();
}

cpp::result<Attacker<CREDIT_SIZE>,std::string> new_Attacker(int budget, const DF<CREDIT_SIZE>& X)
{
    std::filesystem::path attack_file = "";
    auto res = load_attack_rules(json_file);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    Attacker<CREDIT_SIZE> atkr(std::move(rulz), budget);
    Util::info("computing attacks...");
    atkr.compute_attacks(X, attack_file);
    return atkr;
}

// void attack_dataset(const DF<CREDIT_SIZE>& X, ForceCompute force)
// {
//     // const auto attack_file = attack_filename();
//     // if (force == ForceCompute::No && std::filesystem::exists(attack_file))
//     // {
//     //     Util::info("loading attacked dataset from {}...", attack_file.c_str());
//     //     atkr_.load_attacks(attack_file);
//     // }
//     Util::info("computing attacks...");
//     atkr_.compute_attacks<ATTACK_TYPES>(X.df_, attack_file, FEATURE_ID{});
// }

}