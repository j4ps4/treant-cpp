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

template<size_t NX, size_t NY>
std::tuple<DF<NX>,DF<NY>> append3(const std::tuple<DF<NX>,DF<NY>>& fst, 
    const std::tuple<DF<NX>,DF<NY>>& snd, 
    const std::tuple<DF<NX>,DF<NY>>& thrd)
{
    const auto rows1 = std::get<0>(fst).rows();
    const auto rows2 = std::get<0>(snd).rows();
    const auto rows3 = std::get<0>(thrd).rows();
    auto total_rows = rows1 + rows2 + rows3;
    DF<NX> X_out = Eigen::ArrayXXd::Zero(total_rows,NX);
    DF<NY> Y_out = Eigen::ArrayXXd::Zero(total_rows,NY);
    size_t out_idx = 0;
    for (int64_t i = 0; i < rows1; i++)
    {
        X_out.row(out_idx) = std::get<0>(fst).row(i);
        Y_out.row(out_idx++) = std::get<1>(fst).row(i);
    }
    for (int64_t i = 0; i < rows2; i++)
    {
        X_out.row(out_idx) = std::get<0>(snd).row(i);
        Y_out.row(out_idx++) = std::get<1>(snd).row(i);
    }
    for (int64_t i = 0; i < rows3; i++)
    {
        X_out.row(out_idx) = std::get<0>(thrd).row(i);
        Y_out.row(out_idx++) = std::get<1>(thrd).row(i);
    }
    return std::make_tuple(X_out, Y_out);
}

template<size_t NX, size_t NY>
std::tuple<DF<NX>,DF<NY>> append2(const std::tuple<DF<NX>,DF<NY>>& fst, 
    const std::tuple<DF<NX>,DF<NY>>& snd)
{
    const auto rows1 = std::get<0>(fst).rows();
    const auto rows2 = std::get<0>(snd).rows();
    auto total_rows = rows1 + rows2;
    DF<NX> X_out = Eigen::ArrayXXd::Zero(total_rows,NX);
    DF<NY> Y_out = Eigen::ArrayXXd::Zero(total_rows,NY);
    size_t out_idx = 0;
    for (int64_t i = 0; i < rows1; i++)
    {
        X_out.row(out_idx) = std::get<0>(fst).row(i);
        Y_out.row(out_idx++) = std::get<1>(fst).row(i);
    }
    for (int64_t i = 0; i < rows2; i++)
    {
        X_out.row(out_idx) = std::get<0>(snd).row(i);
        Y_out.row(out_idx++) = std::get<1>(snd).row(i);
    }
    return std::make_tuple(X_out, Y_out);
}

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_train()
{
    auto res = df::read_bz2<CREDIT_X,CREDIT_Y>(train_file.c_str()).flat_map([&](const auto& train){
        return df::read_bz2<CREDIT_X,CREDIT_Y>(valid_file.c_str()).map([&](const auto& valid){
            return append2<CREDIT_X,CREDIT_Y>(train, valid);
        });
    });
    return res;
}

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_test()
{
    return df::read_bz2<CREDIT_X,CREDIT_Y>(test_file.c_str());
}

cpp::result<std::unique_ptr<Attacker<CREDIT_X>>,std::string> new_Attacker(int budget, const DF<CREDIT_X>& X)
{
    std::filesystem::path attack_file = "";
    auto res = load_attack_rules(json_file);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_unique<Attacker<CREDIT_X>>(std::move(rulz), budget);
    Util::info("computing attacks...");
    atkr->compute_attacks(X, attack_file);
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


RobustDecisionTree<CREDIT_X,CREDIT_Y> new_RDT(TreeArguments<CREDIT_X,CREDIT_Y>&& args)
{
    return RobustDecisionTree<CREDIT_X,CREDIT_Y>(std::move(args));
}

}