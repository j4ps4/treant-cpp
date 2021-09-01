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
const std::filesystem::path models_dir = data_dir / "models";
const std::filesystem::path train_file = data_dir / "train.csv.bz2";
const std::filesystem::path test_file = data_dir / "test.csv.bz2";
std::filesystem::path default_json_file = data_dir / "attacks.json";
std::filesystem::path json_file;

// std::filesystem::path Hostile::attack_filename() const
// {
//     auto format = fmt::format("credit_B{}.atks", atkr_.get_budget());
//     return data_dir / format;
// }

static const std::map<std::string, size_t> column_map{
    {{ "tBodyAcc-mean()-X",0 },
    { "tBodyAcc-mean()-Y",1 },
    { "tBodyAcc-mean()-Z",2 },
    { "tBodyAcc-std()-X",3 },
    { "tBodyAcc-std()-Y",4 },
    { "tBodyAcc-std()-Z",5 },
    { "tBodyAcc-mad()-X",6 },
    { "tBodyAcc-mad()-Y",7 },
    { "tBodyAcc-mad()-Z",8 },
    { "tBodyAcc-max()-X",9 },
    { "tBodyAcc-max()-Y",10 },
    { "tBodyAcc-max()-Z",11 },
    { "tBodyAcc-min()-X",12 },
    { "tBodyAcc-min()-Y",13 },
    { "tBodyAcc-min()-Z",14 },
    { "tBodyAcc-sma()",15 },
    { "tBodyAcc-energy()-X",16 },
    { "tBodyAcc-energy()-Y",17 },
    { "tBodyAcc-energy()-Z",18 },
    { "tBodyAcc-iqr()-X",19 },
    { "tBodyAcc-iqr()-Y",20 },
    { "tBodyAcc-iqr()-Z",21 },
    { "tBodyAcc-entropy()-X",22 },
    { "tBodyAcc-entropy()-Y",23 },
    { "tBodyAcc-entropy()-Z",24 },
    { "tBodyAcc-arCoeff()-X_1",25 },
    { "tBodyAcc-arCoeff()-X_2",26 },
    { "tBodyAcc-arCoeff()-X_3",27 },
    { "tBodyAcc-arCoeff()-X_4",28 },
    { "tBodyAcc-arCoeff()-Y_1",29 }}};

cpp::result<std::tuple<DF<HAR_X>,DF<HAR_Y>>,std::string> read_train()
{
    return df::read_bz2<HAR_X,HAR_Y,1>(train_file.c_str());
}

cpp::result<std::tuple<DF<HAR_X>,DF<HAR_Y>>,std::string> read_test()
{
    return df::read_bz2<HAR_X,HAR_Y,1>(test_file.c_str());
}

cpp::result<std::unique_ptr<Attacker<HAR_X>>,std::string> new_Attacker(int budget, const DF<HAR_X>& X,
    bool print)
{
    std::filesystem::path& attack_file = default_json_file;
    if (!json_file.empty())
        attack_file = json_file;
    auto res = load_attack_rules(attack_file, column_map);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_unique<Attacker<HAR_X>>(std::move(rulz), budget);
    if (print)
        Util::info("computing attacks...");
    atkr->compute_attacks(X);
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
    size_t min_instances_per_node, int budget, int maxiter, bool affine, int n_inst,
    const std::string& attack_file)
{
    auto m_df = har::read_train();
    if (m_df.has_error())
        Util::die("{}", m_df.error());
    auto& df_tupl = m_df.value();
    auto& X = std::get<0>(df_tupl);
    auto& Y = std::get<1>(df_tupl);

    if (n_inst > 0)
    {
        X.conservativeResize(n_inst, Eigen::NoChange);
        Y.conservativeResize(n_inst, Eigen::NoChange);
    }

    auto m_test = har::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    fmt::print("X: a dataframe of size ({}x{})\n", X.rows(), X.cols());
    fmt::print("Y: a dataframe of size ({}x{})\n", Y.rows(), Y.cols());

    RobustDecisionTree<HAR_X,HAR_Y> tree;

    if (algo != TrainingAlgo::Standard)
    {
        json_file = attack_file;
        auto m_atkr = har::new_Attacker(budget, X);
        if (m_atkr.has_error())
            Util::die("{}", m_atkr.error());
        tree = har::new_RDT({.id = 0, .attacker = std::move(m_atkr.value()),
            .fun = fun, .algo = algo, .max_depth = max_depth, 
            .min_instances_per_node = min_instances_per_node, .maxiter = 100, .affine = affine});
    }
    else
    {
        tree = har::new_RDT({.id = 0, .attacker = nullptr,
            .fun = fun, .algo = algo, .max_depth = max_depth, 
            .min_instances_per_node = min_instances_per_node, .maxiter = 100, .affine = affine});
    }
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    tree.fit(X, Y);
    double linear_time = TIME;
    fmt::print("time elapsed: ");
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
    tree.print_test_score(X_test, Y_test, Y);
    auto model_name = tree.get_model_name();
    auto full_model_name = models_dir / model_name;
    Util::info("saving trained model to {}", full_model_name.native());
    if (!std::filesystem::exists(models_dir))
        std::filesystem::create_directory(models_dir);
    tree.dump_to_disk(full_model_name);
}

void load_and_test(const std::filesystem::path& fn, const std::string& attack_file)
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

    auto tree = RobustDecisionTree<HAR_X,HAR_Y>::load_from_disk(fn);
    tree.print_test_score(X_test, Y_test, Y);

    json_file = attack_file;
    for (int budget : {10,20,30,40,50,60,70,80,90,100})
    {
        auto m_atkr = har::new_Attacker(budget, X_test, false);
        if (m_atkr.has_error())
            Util::die("{}", m_atkr.error());
        auto ptr = m_atkr.value().get();
        double score = tree.get_attacked_score(*ptr, X_test, Y_test);
        fmt::print("budget {}: test score {:.2f}%\n", budget, score);
    }
}

void put_gain_values(const std::filesystem::path& fn)
{
    auto tree = RobustDecisionTree<HAR_X,HAR_Y>::load_from_disk(fn);
    auto gains = tree.feature_importance();

    std::map<double,size_t> ordered;
    for (auto [fid, gain] : gains)
        ordered[gain] = fid;
    fmt::print("Gain\tFeature\n");
    for (auto [gain, fid] : ordered)
        fmt::print("{:.2f}\t{}\n", gain, fid);
}

}