#include <filesystem>
#include <map>

#include "HAR.h"
#include "../AttackerRule.h"
#include "../DF2/DF_read.h"
#include "../DF2/DF_util.h"
#include "../util.h"

namespace har
{

const std::filesystem::path data_dir = "data/har/";
const std::filesystem::path train_file = data_dir / "train.csv.bz2";
const std::filesystem::path test_file = data_dir / "test.csv.bz2";
const std::filesystem::path json_file = data_dir / "attacks.json";

// std::filesystem::path Hostile::attack_filename() const
// {
//     auto format = fmt::format("credit_B{}.atks", atkr_.get_budget());
//     return data_dir / format;
// }

static const std::map<std::string, size_t> column_map{
    {{"tBodyAcc-mean()-X",0},
    {"tBodyAcc-mean()-Y",1}}};

cpp::result<std::tuple<DF<HAR_X>,DF<HAR_Y>>,std::string> read_train()
{
    return df::read_bz2<HAR_X,HAR_Y,1>(train_file.c_str());
}

cpp::result<std::tuple<DF<HAR_X>,DF<HAR_Y>>,std::string> read_test()
{
    return df::read_bz2<HAR_X,HAR_Y,1>(test_file.c_str());
}

cpp::result<std::unique_ptr<Attacker<HAR_X>>,std::string> new_Attacker(int budget, const DF<HAR_X>& X)
{
    std::filesystem::path attack_file = "";
    auto res = load_attack_rules(json_file, column_map);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_unique<Attacker<HAR_X>>(std::move(rulz), budget);
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


RobustDecisionTree<HAR_X,HAR_Y> new_RDT(TreeArguments<HAR_X,HAR_Y>&& args)
{
    return RobustDecisionTree<HAR_X,HAR_Y>(std::move(args));
}

void train_and_test(SplitFunction fun, TrainingAlgo algo, size_t max_depth, 
    size_t min_instances_per_node, int budget, int maxiter, bool affine)
{
    auto m_df = har::read_train();
    if (m_df.has_error())
        Util::die("{}", m_df.error());
    auto& df_tupl = m_df.value();
    auto& X = std::get<0>(df_tupl);
    auto& Y = std::get<1>(df_tupl);

    auto m_test = har::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    auto m_atkr = har::new_Attacker(budget, X);
    if (m_atkr.has_error())
        Util::die("{}", m_atkr.error());
    fmt::print("X: a dataframe of size ({}x{})\n", X.rows(), X.cols());
    fmt::print("Y: a dataframe of size ({}x{})\n", Y.rows(), Y.cols());
    // for (int i = 0; i < 10; i++)
    // {
    //     std::cout << "X: " << X.row(i) << '\n';
    //     std::cout << "Y: " << Y.row(i) << '\n';
    // }
    // std::cout << Y.colwise().mean() << std::endl;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    auto tree = har::new_RDT({.id = 0, .attacker = std::move(m_atkr.value()),
        .fun = fun, .algo = algo, .max_depth = max_depth, 
        .min_instances_per_node = min_instances_per_node, .maxiter = maxiter, .affine = affine});
    tree.fit(X, Y);
    double linear_time = TIME;
    fmt::print("time elapsed: ");
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
    auto Y_pred = tree.predict(X_test);
    fmt::print("Y_test vs. Y_pred:\n");
    for (int i = 0; i < 5; i++)
    {
        std::cout << Y_test.row(i) << " <-> " << Y_pred.row(i) << "\n";
    }
    auto test_acc = 100.0 - 100.0 * tree.classification_error(Y_test, Y_pred);
    auto train_dom = dominant_class<HAR_Y>(Y);
    auto dummy_score = 100.0 * class_proportion<HAR_Y>(Y_test, train_dom);
    fmt::print("test score: {:.2f}% (dummy classifier: {:.2f}%)\n", test_acc, dummy_score);

}

}