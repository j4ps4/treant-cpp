#include <filesystem>
#include <map>

#include "Credit.h"
#include "../AttackerRule.h"
#include "../DF2/DF_read.h"
#include "../DF2/DF_util.h"
#include "../util.h"
#include "templates.tpp"

constexpr size_t CREDIT_X = 23;
constexpr size_t CREDIT_Y = 2;

std::mutex print_mut;
int verbosity = 3;

namespace credit
{

void set_verbosity(int verb) {verbosity = verb;}

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
const static std::set<size_t> DEFAULT_BL;

static const std::vector<double> EPSILON_COEFF{490000.000,0.000,5.000,2.000,40.000,5.000,5.000,5.000,5.000,
5.000,5.000,539726.961,431868.273,507410.805,495597.156,386337.289,637668.438,78464.703,91052.391,84712.023,
79190.734,80594.391,99744.695};

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

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_train_and_valid()
{
    auto res = df::read_bz2<CREDIT_X,CREDIT_Y,0>(train_file.c_str()).flat_map([&](const auto& train){
        return df::read_bz2<CREDIT_X,CREDIT_Y,0>(valid_file.c_str()).map([&](const auto& valid){
            return append2<CREDIT_X,CREDIT_Y>(train, valid);
        });
    });
    return res;
}

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_train()
{
    return df::read_bz2<CREDIT_X,CREDIT_Y,0>(train_file.c_str());
}

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_valid()
{
    return df::read_bz2<CREDIT_X,CREDIT_Y,0>(valid_file.c_str());
}

cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_test()
{
    return df::read_bz2<CREDIT_X,CREDIT_Y,0>(test_file.c_str());
}

cpp::result<std::shared_ptr<Attacker<CREDIT_X>>,std::string> new_Attacker(int budget, const DF<CREDIT_X>& X,
    const std::set<size_t>& id_set, double epsilon)
{
    std::filesystem::path& attack_file = default_json_file;
    if (!json_file.empty())
        attack_file = json_file;
    auto res = load_attack_rules(attack_file, column_map, id_set, epsilon);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_shared<Attacker<CREDIT_X>>(std::move(rulz), budget, std::set<size_t>());
   
    return atkr;
}

void train_and_save(const cxxopts::ParseResult& options)
{
    TRAIN_AND_SAVE(credit, CREDIT_X, CREDIT_Y)
}

void batch_train_and_save(const cxxopts::ParseResult& options)
{
    auto args = generate_arg_from_options<CREDIT_X,CREDIT_Y>(options).value();

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

    auto m_test = credit::read_valid();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    Util::log<4>("X: a dataframe of size ({}x{})", X.rows(), X.cols());
    Util::log<4>("Y: a dataframe of size ({}x{})", Y.rows(), Y.cols());

    auto attackers = parse_batch_file<CREDIT_X>(args.batch_file, args.attack_file, args.budget, args.epsilon);

    if (args.algo == TrainingAlgo::Icml2019)
    {
        auto optimz = std::make_shared<SplitOptimizer<CREDIT_X,CREDIT_Y>>(args.split, args.algo, args.maxiter,
            args.epsilon, args.feature_ids, args.always_ret, args.use_constraints, EPSILON_COEFF);
        args.tree_args.optimizer = std::move(optimz);
    }
    else
    {
        auto optimz = std::make_shared<SplitOptimizer<CREDIT_X,CREDIT_Y>>(args.split, args.algo, args.maxiter, 
            args.epsilon, args.feature_ids, args.always_ret, args.use_constraints);
        args.tree_args.optimizer = std::move(optimz);
    }

    RobustForest<CREDIT_X,CREDIT_Y> forest(args.n_trees, std::move(args.tree_args), attackers);

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    forest.fit(X, Y);
    double linear_time = TIME;
    fmt::print("time elapsed: ");
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
    forest.print_test_score(X_test, Y_test, Y, true);

    auto model_name = args.output.empty() ? forest.get_model_name() : args.output;
    auto full_model_name = models_dir / model_name;
    Util::info("saving trained model to {}", full_model_name.native());
    if (!std::filesystem::exists(models_dir))
        std::filesystem::create_directory(models_dir);
    forest.dump_to_disk(full_model_name);

}

// void cross_val_and_save(TrainArguments<CREDIT_X,CREDIT_Y>&& args, CrossvalArguments&& cv_args)
// {
//     auto m_df = credit::read_train();
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

//     auto m_test = credit::read_test();
//     if (m_test.has_error())
//         Util::die("{}", m_test.error());
//     auto& test_tupl = m_test.value();
//     auto& X_test = std::get<0>(test_tupl);
//     auto& Y_test = std::get<1>(test_tupl);

//     Util::log<4>("X: a dataframe of size ({}x{})", X.rows(), X.cols());
//     Util::log<4>("Y: a dataframe of size ({}x{})", Y.rows(), Y.cols());

//     if (args.algo == TrainingAlgo::Robust)
//     {
//         json_file = args.attack_file;
//         auto m_atkr = credit::new_Attacker(args.budget, X, args.feature_ids);
//         if (m_atkr.has_error())
//             Util::die("{}", m_atkr.error());
//         args.tree_args.attacker = std::move(m_atkr.value());
//     }

//     if (args.algo == TrainingAlgo::Icml2019)
//     {
//         auto optimz = std::make_shared<SplitOptimizer<CREDIT_X,CREDIT_Y>>(args.split, args.algo, args.maxiter,
//             args.epsilon, args.feature_ids, args.always_ret, args.use_constraints, EPSILON_COEFF);
//         args.tree_args.optimizer = std::move(optimz);
//     }
//     else
//     {
//         auto optimz = std::make_shared<SplitOptimizer<CREDIT_X,CREDIT_Y>>(args.split, args.algo, args.maxiter, 
//             args.epsilon, args.feature_ids, args.always_ret, args.use_constraints);
//         args.tree_args.optimizer = std::move(optimz);
//     }

//     const auto L1 = cv_args.maxdepth.size();
//     const auto L2 = cv_args.min_inst.size();
//     const auto L3 = cv_args.affine.size();
//     const auto CV_MATRIX_SIZE = L1*L2*L3;
//     std::vector<TreeArguments<CREDIT_X,CREDIT_Y>> tree_args_v;
//     tree_args_v.reserve(CV_MATRIX_SIZE);
//     for (size_t k = 0; k < L3; k++)
//     {
//         bool affine = cv_args.affine[k];
//         for (size_t j = 0; j < L2; j++)
//         {
//             auto min_inst = cv_args.min_inst[j];
//             for (size_t i = 0; i < L1; i++)
//             {
//                 auto maxdepth = cv_args.maxdepth[i];
//                 TreeArguments<CREDIT_X,CREDIT_Y> prototype = args.tree_args;
//                 prototype.max_depth = maxdepth;
//                 prototype.min_instances_per_node = min_inst;
//                 prototype.affine = affine;
//                 tree_args_v.push_back(std::move(prototype));
//             }
//         }
//     }

//     auto forest = RobustForest<CREDIT_X,CREDIT_Y>(tree_args_v, cv_args.N_folds, cv_args.logfile);

//     std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
//     forest.fit(X, Y);
//     double linear_time = TIME;
//     fmt::print("time elapsed: ");
//     fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
//     forest.print_test_score(X_test, Y_test, Y);
//     auto model_name = args.output.empty() ? forest.get_model_name() : args.output;
//     auto full_model_name = models_dir / model_name;
//     Util::info("saving the best trained model to {}", full_model_name.native());
//     if (!std::filesystem::exists(models_dir))
//         std::filesystem::create_directory(models_dir);
//     forest.dump_to_disk(full_model_name);

//     std::exit(0);
// }

void argument_sweep(const cxxopts::ParseResult& options)
{
    const auto sweep_param = options["sweep-param"].as<std::string>();
    const auto n_inst = options["n-inst"].as<int>();

    auto m_df = credit::read_train();
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

    auto m_test = credit::read_valid();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    Util::log<4>("X: a dataframe of size ({}x{})", X.rows(), X.cols());
    Util::log<4>("Y: a dataframe of size ({}x{})", Y.rows(), Y.cols());

    size_t sweep_index = 0;
    std::optional<TrainArguments<CREDIT_X,CREDIT_Y>> m_arg;

    for (sweep_index = 0, m_arg = generate_arg_from_options<CREDIT_X,CREDIT_Y>(options, sweep_param, sweep_index);
                          m_arg = generate_arg_from_options<CREDIT_X,CREDIT_Y>(options, sweep_param, sweep_index), m_arg.has_value(); 
                          sweep_index++)
    {
        auto arg = m_arg.value();
        fmt::print(fg(fmt::color::green)|fmt::emphasis::bold, 
            "when {} = {}:\n", sweep_param, get_sweep_value(arg, sweep_param));

        if (arg.algo == TrainingAlgo::Robust)
        {
            json_file = arg.attack_file;
            auto m_atkr = credit::new_Attacker(arg.budget, X, arg.feature_ids, arg.epsilon);
            if (m_atkr.has_error())
                Util::die("{}", m_atkr.error());
            arg.tree_args.attacker = std::move(m_atkr.value());
        }

        if (arg.algo == TrainingAlgo::Icml2019)
        {
            auto optimz = std::make_shared<SplitOptimizer<CREDIT_X,CREDIT_Y>>(arg.split, arg.algo, arg.maxiter,
                arg.epsilon, arg.feature_ids, arg.always_ret, arg.use_constraints, EPSILON_COEFF);
            arg.tree_args.optimizer = std::move(optimz);
        }
        else
        {
            auto optimz = std::make_shared<SplitOptimizer<CREDIT_X,CREDIT_Y>>(arg.split, arg.algo, arg.maxiter, 
                arg.epsilon, arg.feature_ids, arg.always_ret, arg.use_constraints);
            arg.tree_args.optimizer = std::move(optimz);
        }

        RobustForest<CREDIT_X,CREDIT_Y> forest(arg.n_trees, std::move(arg.tree_args));

        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        forest.fit(X, Y);
        double linear_time = TIME;
        fmt::print("time elapsed: ");
        fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
        forest.print_test_score(X_test, Y_test, Y, true);
    }
}

void load_and_test(const cxxopts::ParseResult& options, const std::filesystem::path& model_path, int mpi_np, int mpi_rank)
{
    LOAD_AND_TEST(credit, CREDIT_X, CREDIT_Y)
}

void put_gain_values(const std::filesystem::path& fn)
{
    auto forest = RobustForest<CREDIT_X,CREDIT_Y>::load_from_disk(fn);

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
            auto gains = forest.feature_importance();

            std::map<double,size_t> ordered;
            for (auto [fid, gain] : gains)
                ordered[gain] = fid;
            fmt::print("tree {}:\nGain\tFeature\n", i);
            for (auto [gain, fid] : ordered)
                fmt::print("{:.2f}\t{}\n", gain, fid);
        }
    }
}

void classify_inst(const std::filesystem::path& model, const std::vector<double>& inst)
{
    auto forest = RobustForest<CREDIT_X,CREDIT_Y>::load_from_disk(model);
    auto prediction = forest.predict(inst);
    fmt::print("{}\n", prediction);
}

void attack_instance(const std::string& attack_file, const std::vector<double>& inst,
    const std::set<size_t>& id_set, int budget, int cost, double epsilon)
{
    auto res = load_attack_rules(attack_file, column_map, id_set, epsilon);
    if (res.has_error())
        Util::die("{}", res.error());
    auto& rulz = res.value();
    Attacker<CREDIT_X> atkr(std::move(rulz), budget, std::set<size_t>());
    Row<CREDIT_X> rw(1, CREDIT_X);
    if (inst.size() != CREDIT_X)
        Util::die("expected a vector of length {}, got {}", CREDIT_X, inst.size());
    for (size_t i = 0; i < inst.size(); i++)
        rw(i) = inst.at(i);
    const auto& fids = atkr.target_features();
    for (auto fid : fids)
    {
        auto attacks = atkr.adjacent_attack(rw, fid, cost);
        fmt::print("target feature {}:\n", fid);
        for (const auto& [row, c] : attacks)
            std::cout << row << ", cost " << c << "\n";
    }
}

}
