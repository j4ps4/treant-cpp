#include <filesystem>
#include <map>

#include "MNIST.h"
#include "../AttackerRule.h"
#include "../DF2/DF_read.h"
#include "../DF2/DF_util.h"

std::mutex print_mut;
int verbosity = 3;

namespace mnist
{

const static std::set<size_t> MNIST_BL{0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,
    25,26,27,28,29,30,31,52,53,54,55,56,57,82,83,84,85,111,112,140,141,
    168,476,560,644,645,671,672,673,699,700,701,727,728,729,730,754,755,
    756,757,758,759,780,781,782,783};

// const static std::set<size_t> MNIST_BL2{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
// 22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,
// 54,55,56,57,58,59,60,61,62,63,64,78,79,80,81,82,83,84,85,86,87,88,89,90,107,108,109,110,111,112,
// 113,114,115,116,117,137,138,139,140,141,142,143,144,166,167,168,169,170,171,194,195,196,197,198,
// 223,224,225,226,250,251,252,253,278,279,280,281,306,307,308,309,310,334,335,336,337,338,361,362,
// 363,364,365,366,390,391,392,393,394,395,418,419,420,421,422,446,447,448,449,450,474,475,476,477,
// 478,502,503,504,505,506,530,531,532,533,534,558,559,560,561,562,586,587,588,589,590,613,614,615,
// 616,617,618,641,642,643,644,645,646,647,668,669,670,671,672,673,674,675,695,696,697,698,699,700,
// 701,702,703,704,705,722,723,724,725,726,727,728,729,730,731,732,733,749,750,751,752,753,754,755,
// 756,757,758,759,760,761,762,763,764,765,766,767,772,773,774,775,776,777,778,779,780,781,782,783};

void set_verbosity(int verb) {verbosity = verb;}

const std::filesystem::path data_dir = "data/mnist/";
const std::filesystem::path models_dir = data_dir / "models";
const std::filesystem::path train_file_X = data_dir / "train-images.idx3-ubyte.bz2";
const std::filesystem::path train_file_Y = data_dir / "train-labels.idx1-ubyte.bz2";
const std::filesystem::path test_file_X = data_dir / "t10k-images-idx3-ubyte.bz2";
const std::filesystem::path test_file_Y = data_dir / "t10k-labels-idx1-ubyte.bz2";
std::filesystem::path default_json_file = data_dir / "attacks.json";
std::filesystem::path json_file;

static const std::map<std::string, size_t> column_map{};

cpp::result<DF<MNIST_X>,std::string> read_train_X()
{
    return df::read_idx<MNIST_X>(train_file_X.c_str());
}
cpp::result<DF<MNIST_Y>,std::string> read_train_Y()
{
    return df::read_idx_enc<MNIST_Y>(train_file_Y.c_str());
}
cpp::result<DF<MNIST_X>,std::string> read_test_X()
{
    return df::read_idx<MNIST_X>(test_file_X.c_str());
}
cpp::result<DF<MNIST_Y>,std::string> read_test_Y()
{
    return df::read_idx_enc<MNIST_Y>(test_file_Y.c_str());
}

cpp::result<std::shared_ptr<Attacker<MNIST_X>>,std::string> new_Attacker(int budget, const DF<MNIST_X>& X,
    const std::set<size_t>& id_set)
{
    std::filesystem::path& attack_file = default_json_file;
    if (!json_file.empty())
        attack_file = json_file;
    auto res = load_attack_rules(attack_file, column_map, id_set);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_shared<Attacker<MNIST_X>>(std::move(rulz), budget);
   
    return atkr;
}

void train_and_save(TrainArguments<MNIST_X,MNIST_Y>&& args)
{
    auto m_X = mnist::read_train_X();
    if (m_X.has_error())
        Util::die("{}", m_X.error());
    auto& X = m_X.value();
    auto m_Y = mnist::read_train_Y();
    if (m_Y.has_error())
        Util::die("{}", m_Y.error());
    auto& Y = m_Y.value();

    if (args.n_inst > 0)
    {
        X.conservativeResize(args.n_inst, Eigen::NoChange);
        Y.conservativeResize(args.n_inst, Eigen::NoChange);
    }

    auto m_test_X = mnist::read_test_X();
    if (m_test_X.has_error())
        Util::die("{}", m_test_X.error());
    auto& X_test = m_test_X.value();
    auto m_test_Y = mnist::read_test_Y();
    if (m_test_Y.has_error())
        Util::die("{}", m_test_Y.error());
    auto& Y_test = m_test_Y.value();

    Util::log<4>("X: a dataframe of size ({}x{})", X.rows(), X.cols());
    Util::log<4>("Y: a dataframe of size ({}x{})", Y.rows(), Y.cols());

    json_file = args.attack_file;
    auto m_atkr = mnist::new_Attacker(args.budget, X, args.feature_ids);
    if (m_atkr.has_error())
        Util::die("{}", m_atkr.error());
    args.tree_args.attacker = std::move(m_atkr.value());

    auto optimz = std::make_shared<SplitOptimizer<MNIST_X,MNIST_Y>>(args.split, args.algo, args.maxiter,
        args.epsilon, args.feature_ids);
    args.tree_args.optimizer = std::move(optimz);
    args.tree_args.feature_bl = MNIST_BL;

    RobustForest<MNIST_X,MNIST_Y> forest(args.n_trees, std::move(args.tree_args));

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

// void batch_train_and_save(TrainArguments<MNIST_X,MNIST_Y>&& args, const std::string& batch_file)
// {
//     auto m_df = mnist::read_train();
//     if (m_df.has_error())
//         Util::die("{}", m_df.error());
//     auto& df_tupl = m_df.value();
//     auto& X = std::get<0>(df_tupl);
//     auto& Y = std::get<1>(df_tupl);

//     if (args.n_inst > 0)
//     {
//         X.conservativeResize(args.n_inst, Eigen::NoChange);
//         Y.conservativeResize(args.n_inst, Eigen::NoChange);
//     }

//     auto m_test = mnist::read_test();
//     if (m_test.has_error())
//         Util::die("{}", m_test.error());
//     auto& test_tupl = m_test.value();
//     auto& X_test = std::get<0>(test_tupl);
//     auto& Y_test = std::get<1>(test_tupl);

//     Util::log<4>("X: a dataframe of size ({}x{})", X.rows(), X.cols());
//     Util::log<4>("Y: a dataframe of size ({}x{})", Y.rows(), Y.cols());

//     auto attackers = parse_batch_file<MNIST_X>(batch_file, args.attack_file, args.budget);

//     auto optimz = std::make_shared<SplitOptimizer<MNIST_X,MNIST_Y>>(args.split, args.algo, args.maxiter);
//     args.tree_args.optimizer = std::move(optimz);
//     RobustForest<MNIST_X,MNIST_Y> forest(args.n_trees, std::move(args.tree_args), attackers);
//     std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
//     forest.fit(X, Y);
//     double linear_time = TIME;
//     fmt::print("time elapsed: ");
//     fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
//     forest.print_test_score(X_test, Y_test, Y);

//     auto model_name = args.output.empty() ? forest.get_model_name() : args.output;
//     auto full_model_name = models_dir / model_name;
//     Util::info("saving trained model to {}", full_model_name.native());
//     if (!std::filesystem::exists(models_dir))
//         std::filesystem::create_directory(models_dir);
//     forest.dump_to_disk(full_model_name);

//     std::exit(0);
// }

// void load_and_test(const std::filesystem::path& fn, const std::string& attack_file,
//     const std::set<size_t>& id_set, int max_budget)
// {
//     auto m_df = mnist::read_train();
//     if (m_df.has_error())
//         Util::die("{}", m_df.error());
//     auto& df_tupl = m_df.value();
//     auto& X = std::get<0>(df_tupl);
//     auto& Y = std::get<1>(df_tupl);

//     auto m_test = mnist::read_test();
//     if (m_test.has_error())
//         Util::die("{}", m_test.error());
//     auto& test_tupl = m_test.value();
//     auto& X_test = std::get<0>(test_tupl);
//     auto& Y_test = std::get<1>(test_tupl);

//     auto forest = RobustForest<MNIST_X,MNIST_Y>::load_from_disk(fn);
//     forest.print_test_score(X_test, Y_test, Y);

//     std::vector<int> budgets(max_budget);
//     std::iota(budgets.begin(), budgets.end(), 1);

//     if (!attack_file.empty())
//     {
//         json_file = attack_file;
//         for (int budget : budgets)
//         {
//             auto m_atkr = mnist::new_Attacker(budget, X_test, id_set);
//             if (m_atkr.has_error())
//                 Util::die("{}", m_atkr.error());
//             auto ptr = m_atkr.value().get();
//             auto scores = forest.get_attacked_score(*ptr, X_test, Y_test);
//             if (forest.get_type() == ForestType::Bundle)
//             {
//                 for (size_t i = 0; i < scores.size(); i++)
//                 {
//                     auto score = scores[i];
//                     fmt::print("budget {}: tree {}: test score {:.2f}%\n", budget, i, score);
//                 }
//             }
//             else
//             {
//                 auto score = scores[0];
//                 fmt::print("budget {}: test score {:.2f}%\n", budget, score);
//             }
//         }
//     }
//     else
//     {
//         for (int budget : budgets)
//         {
//             forest.set_attacker_budget(budget);
//             auto scores = forest.get_own_attacked_score(X_test, Y_test);
//             if (forest.get_type() == ForestType::Bundle)
//             {
//                 for (size_t i = 0; i < scores.size(); i++)
//                 {
//                     auto score = scores[i];
//                     fmt::print("budget {}: tree {}: test score {:.2f}%\n", budget, i, score);
//                 }
//             }
//             else
//             {
//                 auto score = scores[0];
//                 fmt::print("budget {}: test score {:.2f}%\n", budget, score);
//             }
//         }
//     }
// }

// void put_gain_values(const std::filesystem::path& fn)
// {
//     auto forest = RobustForest<MNIST_X,MNIST_Y>::load_from_disk(fn);

//     if (forest.get_type() == ForestType::Forest)
//     {
//         auto gains = forest.feature_importance();

//         std::map<double,size_t> ordered;
//         for (auto [fid, gain] : gains)
//             ordered[gain] = fid;
//         fmt::print("Gain\tFeature\n");
//         for (auto [gain, fid] : ordered)
//             fmt::print("{:.2f}\t{}\n", gain, fid);
//     }
//     else
//     {
//         const auto N = forest.get_N();
//         for (size_t i = 0; i < N; i++)
//         {
//             auto gains = forest.feature_importance(i);

//             std::map<double,size_t> ordered;
//             for (auto [fid, gain] : gains)
//                 ordered[gain] = fid;
//             fmt::print("tree {}:\nGain\tFeature\n", i);
//             for (auto [gain, fid] : ordered)
//                 fmt::print("{:.2f}\t{}\n", gain, fid);
//         }
//     }
// }

// void classify(const std::filesystem::path& model, const std::vector<double>& inst)
// {
//     auto forest = RobustForest<MNIST_X,MNIST_Y>::load_from_disk(model);
//     auto prediction = forest.predict(inst);
//     fmt::print("{}\n", prediction);
// }

// void attack_instance(const std::string& attack_file, const std::vector<double>& inst,
//     const std::set<size_t>& id_set, int budget, int cost)
// {
//     auto res = load_attack_rules(attack_file, column_map, id_set);
//     if (res.has_error())
//         Util::die("{}", res.error());
//     auto& rulz = res.value();
//     Attacker<MNIST_X> atkr(std::move(rulz), budget);
//     Row<MNIST_X> rw(1, MNIST_X);
//     if (inst.size() != MNIST_X)
//         Util::die("expected a vector of length {}, got {}", MNIST_X, inst.size());
//     for (size_t i = 0; i < inst.size(); i++)
//         rw(i) = inst.at(i);
//     const auto& fids = atkr.target_features();
//     for (auto fid : fids)
//     {
//         auto attacks = atkr.attack(rw, fid, cost);
//         fmt::print("target feature {}:\n", fid);
//         for (const auto& [row, c] : attacks)
//             std::cout << row << ", cost " << c << "\n";
//     }
// }

}