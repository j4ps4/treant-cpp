#include <filesystem>
#include <map>

#include "Credit.h"
#include "../AttackerRule.h"
#include "../DF2/DF_read.h"
#include "../DF2/DF_util.h"
#include "../util.h"

std::mutex print_mut;

namespace credit
{

const std::filesystem::path data_dir = "data/credit/";
const std::filesystem::path models_dir = data_dir / "models";
const std::filesystem::path train_file = data_dir / "train.csv.bz2";
const std::filesystem::path valid_file = data_dir / "valid.csv.bz2";
const std::filesystem::path test_file = data_dir / "test.csv.bz2";
std::filesystem::path default_json_file = data_dir / "attacks.json";
std::filesystem::path json_file;

// std::filesystem::path Hostile::attack_filename() const
// {
//     auto format = fmt::format("credit_B{}.atks", atkr_.get_budget());
//     return data_dir / format;
// }

static const std::map<std::string, size_t> column_map{
    {{"LIMIT_BAL",0},
    {"SEX",1},
    {"EDUCATION",2},
    {"MARRIAGE",3},
    {"AGE",4},
    {"PAY_0",5},
    {"PAY_2",6},
    {"PAY_3",7},
    {"PAY_4",8},
    {"PAY_5",9},
    {"PAY_6",10},
    {"BILL_AMT1",11},
    {"BILL_AMT2",12},
    {"BILL_AMT3",13},
    {"BILL_AMT4",14},
    {"BILL_AMT5",15},
    {"BILL_AMT6",16},
    {"PAY_AMT1",17},
    {"PAY_AMT2",18},
    {"PAY_AMT3",19},
    {"PAY_AMT4",20},
    {"PAY_AMT5",21},
    {"PAY_AMT6",22},
    {"default.payment.next.month",23}}};

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_train()
{
    auto res = df::read_bz2<CREDIT_X,CREDIT_Y,0>(train_file.c_str()).flat_map([&](const auto& train){
        return df::read_bz2<CREDIT_X,CREDIT_Y,0>(valid_file.c_str()).map([&](const auto& valid){
            return append2<CREDIT_X,CREDIT_Y>(train, valid);
        });
    });
    return res;
}

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_test()
{
    return df::read_bz2<CREDIT_X,CREDIT_Y,0>(test_file.c_str());
}

cpp::result<std::unique_ptr<Attacker<CREDIT_X>>,std::string> new_Attacker(int budget, const DF<CREDIT_X>& X,
    const std::set<size_t>& id_set, bool print = true)
{
    std::filesystem::path& attack_file = default_json_file;
    if (!json_file.empty())
        attack_file = json_file;
    auto res = load_attack_rules(attack_file, column_map, id_set);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_unique<Attacker<CREDIT_X>>(std::move(rulz), budget);
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


RobustDecisionTree<CREDIT_X,CREDIT_Y> new_RDT(TreeArguments<CREDIT_X,CREDIT_Y>&& args)
{
    return RobustDecisionTree<CREDIT_X,CREDIT_Y>(std::move(args));
}

void train_and_save(TrainArguments<CREDIT_X,CREDIT_Y>&& args)
{
    auto m_df = credit::read_train();
    if (m_df.has_error())
        Util::die("{}", m_df.error());
    auto& df_tupl = m_df.value();
    auto& X = std::get<0>(df_tupl);
    auto& Y = std::get<1>(df_tupl);

    if (args.n_inst > 0)
    {
        X.conservativeResize(args.n_inst, Eigen::NoChange);
        Y.conservativeResize(args.n_inst, Eigen::NoChange);
    }

    auto m_test = credit::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    fmt::print("X: a dataframe of size ({}x{})\n", X.rows(), X.cols());
    fmt::print("Y: a dataframe of size ({}x{})\n", Y.rows(), Y.cols());

    RobustDecisionTree<CREDIT_X,CREDIT_Y> tree;

    if (args.tree_args.algo != TrainingAlgo::Standard)
    {
        json_file = args.attack_file;
        auto m_atkr = credit::new_Attacker(args.budget, X, args.feature_ids);
        if (m_atkr.has_error())
            Util::die("{}", m_atkr.error());
        args.tree_args.attacker = std::move(m_atkr.value());
        tree = credit::new_RDT(std::move(args.tree_args));
    }
    else
    {
        tree = credit::new_RDT(std::move(args.tree_args));
    }

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    tree.fit(X, Y);
    double linear_time = TIME;
    fmt::print("time elapsed: ");
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
    tree.print_test_score(X_test, Y_test, Y);
    auto model_name = args.output.empty() ? tree.get_model_name() : args.output;
    auto full_model_name = models_dir / model_name;
    Util::info("saving trained model to {}", full_model_name.native());
    if (!std::filesystem::exists(models_dir))
        std::filesystem::create_directory(models_dir);
    tree.dump_to_disk(full_model_name);
}

void load_and_test(const std::filesystem::path& fn, const std::string& attack_file,
    const std::set<size_t>& id_set)
{
    auto m_df = credit::read_train();
    if (m_df.has_error())
        Util::die("{}", m_df.error());
    auto& df_tupl = m_df.value();
    auto& X = std::get<0>(df_tupl);
    auto& Y = std::get<1>(df_tupl);

    auto m_test = credit::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    auto tree = RobustDecisionTree<CREDIT_X,CREDIT_Y>::load_from_disk(fn);
    tree.print_test_score(X_test, Y_test, Y);

    auto budgets = {1,2,3,4,5};

    if (!attack_file.empty())
    {
        json_file = attack_file;
        for (int budget : budgets)
        {
            auto m_atkr = credit::new_Attacker(budget, X_test, id_set, false);
            if (m_atkr.has_error())
                Util::die("{}", m_atkr.error());
            auto ptr = m_atkr.value().get();
            double score = tree.get_attacked_score(*ptr, X_test, Y_test);
            fmt::print("budget {}: test score {:.2f}%\n", budget, score);
        }
    }
    else
    {
        for (int budget : budgets)
        {
            tree.set_attacker_budget(budget);
            double score = tree.get_own_attacked_score(X_test, Y_test);
            fmt::print("budget {}: test score {:.2f}%\n", budget, score);
        }
    }
}

void put_gain_values(const std::filesystem::path& fn)
{
    auto tree = RobustDecisionTree<CREDIT_X,CREDIT_Y>::load_from_disk(fn);
    auto gains = tree.feature_importance();

    std::map<double,size_t> ordered;
    for (auto [fid, gain] : gains)
        ordered[gain] = fid;
    fmt::print("Gain\tFeature\n");
    for (auto [gain, fid] : ordered)
        fmt::print("{:.2f}\t{}\n", gain, fid);
}

void classify(const std::filesystem::path& model, const std::vector<double>& inst)
{
    auto tree = RobustDecisionTree<CREDIT_X,CREDIT_Y>::load_from_disk(model);
    auto prediction = tree.predict(inst);
    fmt::print("{}\n", prediction);
}

void attack_instance(const std::string& attack_file, const std::vector<double>& inst,
    const std::set<size_t>& id_set, int budget, int cost)
{
    auto res = load_attack_rules(attack_file, column_map, id_set);
    if (res.has_error())
        Util::die("{}", res.error());
    auto& rulz = res.value();
    Attacker<CREDIT_X> atkr(std::move(rulz), budget);
    Row<CREDIT_X> rw(1, CREDIT_X);
    if (inst.size() != CREDIT_X)
        Util::die("expected a vector of length {}, got {}", CREDIT_X, inst.size());
    for (size_t i = 0; i < inst.size(); i++)
        rw(i) = inst.at(i);
    const auto& fids = atkr.target_features();
    for (auto fid : fids)
    {
        auto attacks = atkr.attack(rw, fid, cost);
        fmt::print("target feature {}:\n", fid);
        for (const auto& [row, c] : attacks)
            std::cout << row << ", cost " << c << "\n";
    }
}

}
