#include <filesystem>
#include <map>

#include "HAR.h"
#include "../AttackerRule.h"
#include "../DF2/DF_read.h"
#include "../DF2/DF_util.h"
#include "../util.h"

std::mutex print_mut;
int verbosity = 3;

namespace har
{

void set_verbosity(int verb) {verbosity = verb;}

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

cpp::result<std::shared_ptr<Attacker<HAR_X>>,std::string> new_Attacker(int budget, const DF<HAR_X>& X,
    const std::set<size_t>& id_set)
{
    std::filesystem::path& attack_file = default_json_file;
    if (!json_file.empty())
        attack_file = json_file;
    auto res = load_attack_rules(attack_file, column_map, id_set);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_shared<Attacker<HAR_X>>(std::move(rulz), budget);

    return atkr;
}

void train_and_save(TrainArguments<HAR_X,HAR_Y>&& args)
{
    auto m_df = har::read_train();
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

    auto m_test = har::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    Util::log<4>("X: a dataframe of size ({}x{})\n", X.rows(), X.cols());
    Util::log<4>("Y: a dataframe of size ({}x{})\n", Y.rows(), Y.cols());

    json_file = args.attack_file;
    auto m_atkr = har::new_Attacker(args.budget, X, args.feature_ids);
    if (m_atkr.has_error())
        Util::die("{}", m_atkr.error());
    args.tree_args.attacker = std::move(m_atkr.value());

    auto optimz = std::make_shared<SplitOptimizer<HAR_X,HAR_Y>>(args.split, args.algo, args.maxiter);
    args.tree_args.optimizer = std::move(optimz);

    RobustForest<HAR_X,HAR_Y> forest(args.n_trees, std::move(args.tree_args));

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    forest.fit(X, Y);
    double linear_time = TIME;
    fmt::print("time elapsed: ");
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
    forest.print_test_score(X_test, Y_test, Y);
    auto model_name = args.output.empty() ? forest.get_model_name() : args.output;
    auto full_model_name = models_dir / model_name;
    Util::info("saving trained model to {}", full_model_name.native());
    if (!std::filesystem::exists(models_dir))
        std::filesystem::create_directory(models_dir);
    forest.dump_to_disk(full_model_name);
}

void batch_train_and_save(TrainArguments<HAR_X,HAR_Y>&& args, const std::string& batch_file)
{
    auto m_df = har::read_train();
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

    auto m_test = har::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    Util::log<4>("X: a dataframe of size ({}x{})\n", X.rows(), X.cols());
    Util::log<4>("Y: a dataframe of size ({}x{})\n", Y.rows(), Y.cols());

    auto attackers = parse_batch_file<HAR_X>(batch_file, args.attack_file, args.budget);

    auto optimz = std::make_shared<SplitOptimizer<HAR_X,HAR_Y>>(args.split, args.algo, args.maxiter);
    args.tree_args.optimizer = std::move(optimz);
    RobustForest<HAR_X,HAR_Y> forest(args.n_trees, std::move(args.tree_args), attackers);
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    forest.fit(X, Y);
    double linear_time = TIME;
    fmt::print("time elapsed: ");
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
    forest.print_test_score(X_test, Y_test, Y);

    auto model_name = args.output.empty() ? forest.get_model_name() : args.output;
    auto full_model_name = models_dir / model_name;
    Util::info("saving trained model to {}", full_model_name.native());
    if (!std::filesystem::exists(models_dir))
        std::filesystem::create_directory(models_dir);
    forest.dump_to_disk(full_model_name);

    std::exit(0);
}

void load_and_test(const std::filesystem::path& fn, const std::string& attack_file,
    const std::set<size_t>& id_set, int max_budget)
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

    auto forest = RobustForest<HAR_X,HAR_Y>::load_from_disk(fn);
    forest.print_test_score(X_test, Y_test, Y);

    std::vector<int> budgets(max_budget);
    std::iota(budgets.begin(), budgets.end(), 1);

    if (!attack_file.empty())
    {
        json_file = attack_file;
        for (int budget : budgets)
        {
            auto m_atkr = har::new_Attacker(budget, X_test, id_set);
            if (m_atkr.has_error())
                Util::die("{}", m_atkr.error());
            auto ptr = m_atkr.value().get();
            auto scores = forest.get_attacked_score(*ptr, X_test, Y_test);
            if (forest.get_type() == ForestType::Bundle)
            {
                for (size_t i = 0; i < scores.size(); i++)
                {
                    auto score = scores[i];
                    fmt::print("budget {}: tree {}: test score {:.2f}%\n", budget, i, score);
                }
            }
            else
            {
                auto score = scores[0];
                fmt::print("budget {}: test score {:.2f}%\n", budget, score);
            }
        }
    }
    else
    {
        for (int budget : budgets)
        {
            forest.set_attacker_budget(budget);
            auto scores = forest.get_own_attacked_score(X_test, Y_test);
            if (forest.get_type() == ForestType::Bundle)
            {
                for (size_t i = 0; i < scores.size(); i++)
                {
                    auto score = scores[i];
                    fmt::print("budget {}: tree {}: test score {:.2f}%\n", budget, i, score);
                }
            }
            else
            {
                auto score = scores[0];
                fmt::print("budget {}: test score {:.2f}%\n", budget, score);
            }
        }
    }
}

void put_gain_values(const std::filesystem::path& fn)
{
    auto forest = RobustForest<HAR_X,HAR_Y>::load_from_disk(fn);

    if (forest.get_type() == ForestType::Forest)
    {
        auto gains = forest.feature_importance();

        std::map<double,size_t> ordered;
        for (auto [fid, gain] : gains)
            ordered[gain] = fid;
        fmt::print("Gain\tFeature\n");
        for (auto [gain, fid] : ordered)
            fmt::print("{:.2f}\t{}\n", gain, fid);
    }
    else
    {
        const auto N = forest.get_N();
        for (size_t i = 0; i < N; i++)
        {
            auto gains = forest.feature_importance(i);

            std::map<double,size_t> ordered;
            for (auto [fid, gain] : gains)
                ordered[gain] = fid;
            fmt::print("tree {}:\nGain\tFeature\n", i);
            for (auto [gain, fid] : ordered)
                fmt::print("{:.2f}\t{}\n", gain, fid);
        }
    }
}

void classify(const std::filesystem::path& model, const std::vector<double>& inst)
{
    auto forest = RobustForest<HAR_X,HAR_Y>::load_from_disk(model);
    auto prediction = forest.predict(inst);
    fmt::print("{}\n", prediction);
}

void attack_instance(const std::string& attack_file, const std::vector<double>& inst,
    const std::set<size_t>& id_set, int budget, int cost)
{
    auto res = load_attack_rules(attack_file, column_map, id_set);
    if (res.has_error())
        Util::die("{}", res.error());
    auto& rulz = res.value();
    Attacker<HAR_X> atkr(std::move(rulz), budget);
    Row<HAR_X> rw(1, HAR_X);
    if (inst.size() != HAR_X)
        Util::die("expected a vector of length {}, got {}", HAR_X, inst.size());
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