#include <filesystem>
#include <map>

#include "MNIST.h"
#include "../AttackerRule.h"
#include "../DF2/DF_read.h"
#include "../DF2/DF_util.h"
#include "templates.tpp"

constexpr size_t MNIST_X = 28*28;
constexpr size_t MNIST_Y = 10;

std::mutex print_mut;
int verbosity = 3;

namespace mnist
{

// const static std::set<size_t> MNIST_BL{0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,
//     25,26,27,28,29,30,31,52,53,54,55,56,57,82,83,84,85,111,112,140,141,
//     168,476,560,644,645,671,672,673,699,700,701,727,728,729,730,754,755,
//     756,757,758,759,780,781,782,783};

const static std::set<size_t> DEFAULT_BL{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,
54,55,56,57,58,59,60,61,62,63,64,78,79,80,81,82,83,84,85,86,87,88,89,90,107,108,109,110,111,112,
113,114,115,116,117,137,138,139,140,141,142,143,144,166,167,168,169,170,171,194,195,196,197,198,
223,224,225,226,250,251,252,253,278,279,280,281,306,307,308,309,310,334,335,336,337,338,361,362,
363,364,365,366,390,391,392,393,394,395,418,419,420,421,422,446,447,448,449,450,474,475,476,477,
478,502,503,504,505,506,530,531,532,533,534,558,559,560,561,562,586,587,588,589,590,613,614,615,
616,617,618,641,642,643,644,645,646,647,668,669,670,671,672,673,674,675,695,696,697,698,699,700,
701,702,703,704,705,722,723,724,725,726,727,728,729,730,731,732,733,749,750,751,752,753,754,755,
756,757,758,759,760,761,762,763,764,765,766,767,772,773,774,775,776,777,778,779,780,781,782,783};

static const std::vector<double> EPSILON_COEFF{0,0,0,0,0,0,0,0,0,0,0,0,116,254,216,9,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,16,47,157,254,255,254,255,255,255,255,255,255,255,255,255,255,244,255,184,197,
0,0,0,0,0,0,64,29,134,62,234,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,165,0,0,0,0,141,101,96,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,192,121,0,0,38,144,101,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,254,221,63,0,0,95,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,93,0,7,210,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,254,253,47,191,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,253,191,252,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,221,184,254,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,220,214,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,203,150,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,
131,163,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,254,252,152,32,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,254,252,247,113,188,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,254,52,37,226,254,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,190,40,107,254,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,254,223,0,133,254,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,104,60,197,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,153,15,166,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,128,0,185,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,51,32,185,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,39,31,38,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,225,72,
0,0,217,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,150,
0,0,0,253,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,252,
98,0,0,0,42,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,127,
104,0,0,0,0,38,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,137,28,59,0,
0,0,0,0,0,94,252,245,254,254,255,255,255,255,255,255,255,255,255,254,254,253,253,254,62,0,0,0,0};

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

cpp::result<std::tuple<DF<MNIST_X>,DF<MNIST_Y>>,std::string> read_train_and_valid()
{
    auto res = df::read_idx<MNIST_X>(train_file_X.c_str()).flat_map([&](const auto& train_X){
        return df::read_idx_enc<MNIST_Y>(train_file_Y.c_str()).map([&](const auto& train_Y){
            return std::make_tuple(train_X, train_Y);
        });
    });
    return res;
}

cpp::result<std::tuple<DF<MNIST_X>,DF<MNIST_Y>>,std::string> read_train()
{
    auto res = df::read_idx<MNIST_X>(train_file_X.c_str()).flat_map([&](const auto& train_X){
        return df::read_idx_enc<MNIST_Y>(train_file_Y.c_str()).map([&](const auto& train_Y){
            return std::make_tuple(train_X, train_Y);
        });
    });
    return res;
}

cpp::result<std::tuple<DF<MNIST_X>,DF<MNIST_Y>>,std::string> read_valid()
{
    auto res = df::read_idx<MNIST_X>(test_file_X.c_str()).flat_map([&](const auto& test_X){
        return df::read_idx_enc<MNIST_Y>(test_file_Y.c_str()).map([&](const auto& test_Y){
            return std::make_tuple(test_X, test_Y);
        });
    });
    return res;
}

cpp::result<std::tuple<DF<MNIST_X>,DF<MNIST_Y>>,std::string> read_test()
{
    auto res = df::read_idx<MNIST_X>(test_file_X.c_str()).flat_map([&](const auto& test_X){
        return df::read_idx_enc<MNIST_Y>(test_file_Y.c_str()).map([&](const auto& test_Y){
            return std::make_tuple(test_X, test_Y);
        });
    });
    return res;
}

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
    const std::set<size_t>& id_set, double epsilon)
{
    std::filesystem::path& attack_file = default_json_file;
    if (!json_file.empty())
        attack_file = json_file;
    auto res = load_attack_rules(attack_file, column_map, id_set, epsilon);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    auto atkr = std::make_shared<Attacker<MNIST_X>>(std::move(rulz), budget, DEFAULT_BL);
   
    return atkr;
}

void train_and_save(const cxxopts::ParseResult& options)
{
    TRAIN_AND_SAVE(mnist, MNIST_X, MNIST_Y)
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

void argument_sweep(const cxxopts::ParseResult& options)
{
    const auto sweep_param = options["sweep-param"].as<std::string>();
    const auto n_inst = options["n-inst"].as<int>();

    auto m_X = mnist::read_train_X();
    if (m_X.has_error())
        Util::die("{}", m_X.error());
    auto& X = m_X.value();
    auto m_Y = mnist::read_train_Y();
    if (m_Y.has_error())
        Util::die("{}", m_Y.error());
    auto& Y = m_Y.value();

    if (n_inst > 0)
    {
        X.conservativeResize(n_inst, Eigen::NoChange);
        Y.conservativeResize(n_inst, Eigen::NoChange);
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

    size_t sweep_index = 0;
    std::optional<TrainArguments<MNIST_X,MNIST_Y>> m_arg;

    for (sweep_index = 0, m_arg = generate_arg_from_options<MNIST_X,MNIST_Y>(options, sweep_param, sweep_index);
                          m_arg = generate_arg_from_options<MNIST_X,MNIST_Y>(options, sweep_param, sweep_index), m_arg.has_value(); 
                          sweep_index++)
    {
        auto arg = m_arg.value();
        fmt::print(fg(fmt::color::green)|fmt::emphasis::bold, 
            "when {} = {}:\n", sweep_param, get_sweep_value(arg, sweep_param));

        if (arg.algo == TrainingAlgo::Robust)
        {
            json_file = arg.attack_file;
            auto m_atkr = mnist::new_Attacker(arg.budget, X, arg.feature_ids, arg.epsilon);
            if (m_atkr.has_error())
                Util::die("{}", m_atkr.error());
            arg.tree_args.attacker = std::move(m_atkr.value());
        }

        if (arg.algo == TrainingAlgo::Icml2019)
        {
            auto optimz = std::make_shared<SplitOptimizer<MNIST_X,MNIST_Y>>(arg.split, arg.algo, arg.maxiter,
                arg.epsilon, arg.feature_ids, arg.always_ret, arg.use_constraints, EPSILON_COEFF);
            arg.tree_args.optimizer = std::move(optimz);
        }
        else
        {
            auto optimz = std::make_shared<SplitOptimizer<MNIST_X,MNIST_Y>>(arg.split, arg.algo, arg.maxiter, 
                arg.epsilon, arg.feature_ids, arg.always_ret, arg.use_constraints);
            arg.tree_args.optimizer = std::move(optimz);
        }

        RobustForest<MNIST_X,MNIST_Y> forest(arg.n_trees, std::move(arg.tree_args));

        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        forest.fit(X, Y);
        double linear_time = TIME;
        fmt::print("time elapsed: ");
        fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time));
        forest.print_test_score(X_test, Y_test, Y, false);
    }
}

void load_and_test(const cxxopts::ParseResult& options, const std::filesystem::path& model_path, int mpi_np, int mpi_rank)
{
    LOAD_AND_TEST(mnist, MNIST_X, MNIST_Y)
}

void put_gain_values(const std::filesystem::path& fn)
{
    auto forest = RobustForest<MNIST_X,MNIST_Y>::load_from_disk(fn);

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
    auto forest = RobustForest<MNIST_X,MNIST_Y>::load_from_disk(model);
    auto prediction = forest.predict(inst);
    auto probs = forest.predict_proba_row(inst);
    auto prob = probs[prediction];
    fmt::print("predicted label = {}, propability = {:.4f}\n", prediction, prob);
}

void classify(const std::filesystem::path& model, const size_t inst_id)
{
    auto forest = RobustForest<MNIST_X,MNIST_Y>::load_from_disk(model);

    auto m_test = mnist::read_test();
    if (m_test.has_error()) \
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);
    const auto& row = X_test.row(inst_id);
    Eigen::Index y_true;
    Y_test.row(inst_id).maxCoeff(&y_true);

    auto prediction = forest.predict(row);
    auto probs = forest.predict_proba_row(row);
    auto prob = probs[prediction];
    fmt::print("true label = {}, predicted label = {}, propability = {:.4f}\n", y_true, prediction, prob);
}

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

bool blackbox(const std::filesystem::path& model, const cxxopts::ParseResult& options)
{
    const auto index = options["blackbox"].as<size_t>();
    const int samples = options["budget"].as<std::vector<int>>().front();
    const int noise_level = options["verbosity"].as<int>();
    auto m_df = mnist::read_train_and_valid();
    if (m_df.has_error())
        Util::die("{}", m_df.error());
    auto& df_tupl = m_df.value();
    auto& X = std::get<0>(df_tupl);
    auto& Y = std::get<1>(df_tupl);

    auto m_test = mnist::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    auto forest = RobustForest<MNIST_X,MNIST_Y>::load_from_disk(model);

    if (!options.count("n-inst"))
    {
        fmt::print(stderr, "Blackbox attack against instance #{}:\n", index);
        if (index >= X_test.rows())
        {
            fmt::print("Index too large. ({} > {})\n", index+1, X_test.rows());
            return false;
        }
        const auto& inst = X_test.row(index);
        Eigen::Index y_true;
        Y_test.row(index).maxCoeff(&y_true);
        if (forest.predict(inst) != y_true)
        {
            fmt::print("Misclassification.\n");
            return false;
        }

        fmt::print(stderr, "Original label: {}\n", y_true);

        auto [deformed, distortion] = forest.blackbox_attack(X, Y, X_test, Y_test, index,
            noise_level <= 3, samples);

        auto pred = forest.predict(deformed);
        auto probs = forest.predict_proba_row(deformed);
        auto prob = probs[pred];
        if (noise_level > 4)
            fmt::print("true label = {}, predicted label = {}, propability = {:.4f}, distortion = {:.4f}, deformed = {}\n",
                y_true, pred, prob, distortion, row_str<MNIST_X,12>(deformed));
        else
            fmt::print("true label = {}, predicted label = {}, propability = {:.4f}, distortion = {:.4f}\n",
                y_true, pred, prob, distortion);
    }
    else
    {
        const int n_inst = options["n-inst"].as<int>();
        fmt::print(stderr, "Blackbox attack against first {} instances:\n", n_inst);
        double total = 0.0;
        int corrects = 0;
        for (int ix = 0; ix < n_inst; ix++)
        {
            const auto& inst = X_test.row(ix);
            Eigen::Index y_true;
            Y_test.row(ix).maxCoeff(&y_true);
            if (forest.predict(inst) != y_true)
            {
                continue;
            }
            corrects++;
            auto [deformed, distortion] = forest.blackbox_attack(X, Y, X_test, Y_test, ix,
                noise_level <= 3, samples);
            total += distortion;
        }
        total = total / corrects;
        fmt::print("avg. distortion = {}\n", total);
    }
    return true;
}

}
