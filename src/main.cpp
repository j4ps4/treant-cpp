#include <fmt/core.h>
#include <cxxopts.hpp>
#include <stdio.h>
#include <ranges>
#include <cctype>
#include <tuple>
#include <filesystem>

#include "dataset/Credit.h"
#include "dataset/Covertype.h"
#include "dataset/HAR.h"
#include "dataset/MNIST.h"
#include "util.h"

enum class DataSet
{
    Credit, Har, Covertype, Mnist
};

std::tuple<DataSet, std::filesystem::path> parseTest(const std::string& s)
{
    const std::filesystem::path p(s);
    auto beg = p.begin();
    std::advance(beg, 1);
    const auto& dirname = *beg;
    if (dirname == "credit")
        return {DataSet::Credit, p};
    else if (dirname == "har")
        return {DataSet::Har, p};
    else if (dirname == "covertype")
        return {DataSet::Covertype, p};
    else if (dirname == "mnist")
        return {DataSet::Mnist, p};
    else
        Util::die("model path is not valid");
}

DataSet parseAttack(const std::string& s)
{
    const std::filesystem::path p(s);
    auto beg = p.begin();
    std::advance(beg, 1);
    const auto& dirname = *beg;
    if (dirname == "credit")
        return DataSet::Credit;
    else if (dirname == "har")
        return DataSet::Har;
    else if (dirname == "covertype")
        return DataSet::Covertype;
    else if (dirname == "mnist")
        return DataSet::Mnist;
    else
        Util::die("attack_file path is not valid");
}
 
template<typename Range>
void toLower(Range& rang)
{
    std::ranges::transform(rang.begin(), rang.end(), rang.begin(),
                   [](unsigned char c) -> unsigned char { return std::tolower(c); });
}

int main(int argc, char** argv)
{
    cxxopts::Options options("treant", "robust tree classifier\ndefault mode is training.");

    options.add_options("mode")
        ("test", "perform testing on a trained model",
        cxxopts::value<bool>()->default_value("false"))
        ("sweep", "perform a parameter sweep",
        cxxopts::value<bool>()->default_value("false"))
        ("sweep-param", "sweep parameter",
        cxxopts::value<std::string>())
        ("gain", "compute feature importance values for a trained model",
        cxxopts::value<bool>()->default_value("false"))
        ("cross-val", "perform N-fold cross-validation",
        cxxopts::value<size_t>())
        ("classify", "classify given instance, comma separated list of values",
        cxxopts::value<std::vector<double>>())
        ("attack", "attack given instance, comma separated list of values",
        cxxopts::value<std::vector<double>>())
        ("batch", "train a number of trees in batch mode, using given batch file",
        cxxopts::value<std::string>())
        ("V,verbosity", "output verbosity: 0 (nothing) - 4 (everything)",
        cxxopts::value<int>()->default_value("3"))

        ("h,help", "print usage");

    options.add_options("file")
        ("data", "dataset to use",
        cxxopts::value<std::string>())
        ("O,output", "model output name",
        cxxopts::value<std::string>()->default_value(""))
        ("M,model", "trained model on which to operate",
        cxxopts::value<std::string>())
        ("L,log-file", "output log file",
        cxxopts::value<std::string>()->default_value(""))
        ("A,attack-file", "json file describing the attacks",
        cxxopts::value<std::string>()->default_value(""));

    options.add_options("tree")
        ("algo", "training algorithm: robust (TREANT), icml2019 (Chen et al.), standard",
        cxxopts::value<std::string>()->default_value("robust"))
        ("B,budget", "maximum budget of attacker",
        cxxopts::value<std::vector<int>>()->default_value("5"))
        ("maxiter", "maximum nlopt iterations",
        cxxopts::value<int>()->default_value("100"))
        ("epsilon", "radius of the l-inf ball (Chen et al.)",
        cxxopts::value<std::vector<double>>()->default_value("0.3"))
        ("D,maxdepth", "maximum depth of tree",
        cxxopts::value<std::vector<size_t>>()->default_value("8"))
        ("min-instances-per-node", "minimum instances per leaf node",
        cxxopts::value<std::vector<size_t>>()->default_value("20"))
        ("N,n-trees", "amount of trees in ensemble",
        cxxopts::value<size_t>()->default_value("1"))
        ("n-inst", "number of instances to use (for training)",
        cxxopts::value<int>()->default_value("-1"))
        ("n-feats", "number of features to sample (for testing)",
        cxxopts::value<int>()->default_value("-1"))
        ("sample-instances", "sample instances when training an ensemble",
        cxxopts::value<bool>()->default_value("false"))
        ("sample-features", "sample features when training an ensemble",
        cxxopts::value<bool>()->default_value("false"))
        ("replace-instances", "whether the random sampling of instances should be with replacement",
        cxxopts::value<bool>()->default_value("false"))
        ("instance-prop", "proportion of instances sampled",
        cxxopts::value<std::vector<double>>()->default_value("0.8"))
        ("feature-prop", "proportion of features sampled",
        cxxopts::value<std::vector<double>>()->default_value("0.8"))
        ("affine", "don't consider the same features again when growing tree downward",
        cxxopts::value<std::vector<bool>>()->default_value("true"))
        ("par", "train single tree in parallel mode",
        cxxopts::value<bool>()->default_value("false"))
        ("feature-blacklist", "features to ignore when considering a split",
        cxxopts::value<std::vector<size_t>>()->default_value(""))
        ("feature-ids", "features to use in ID# rules, and Chen et al.",
        cxxopts::value<std::vector<size_t>>()->default_value(""))
        ("nlopt-always-return", "should nlopt return a value even when thrown exception",
        cxxopts::value<bool>()->default_value("true"))
        ("use-constraints", "should robust use invariance constraints",
        cxxopts::value<bool>()->default_value("true"));

    options.add_options("internal")
        ("replace-features", "whether the random sampling of features should be with replacement",
        cxxopts::value<bool>()->default_value("false"))
        ("cost", "starting cost",
        cxxopts::value<int>()->default_value("0"))
        ("par-par", "internal parallel mode argument",
        cxxopts::value<double>()->default_value("0.5"));
    
    try
    {
        auto opts = options.parse(argc, argv);
        if (opts.count("help"))
        {
            fmt::print("{}\n", options.help({"mode","file","tree"}));
            exit(0);
        }

        auto check_model = [](const auto& arg)->std::string{
            if (!arg.count("model"))
                throw std::invalid_argument("missing --model");
            return arg["model"].template as<std::string>();
        };

        auto feature_id_vec = opts["feature-ids"].as<std::vector<size_t>>();
        std::set<size_t> feature_id;
        for (auto f : feature_id_vec)
            feature_id.insert(f);

        auto attack_file = opts["attack-file"].as<std::string>();
        auto budget_v = opts["budget"].as<std::vector<int>>();
        auto budget = budget_v.front();
        auto cost = opts["cost"].as<int>();
        auto n_inst = opts["n-inst"].as<int>();
        auto n_feats = opts["n-feats"].as<int>();
        auto epsilon_v = opts["epsilon"].as<std::vector<double>>();
        auto epsilon = epsilon_v.front();
        if (opts.count("test"))
        {
            auto mod = check_model(opts);
            auto [dataset, path] = parseTest(mod);
            if (dataset == DataSet::Credit)
                credit::load_and_test(path, attack_file, feature_id, budget, n_inst, n_feats, epsilon);
            else if (dataset == DataSet::Har)
                har::load_and_test(path, attack_file, feature_id, budget, n_inst, n_feats, epsilon);
            else if (dataset == DataSet::Covertype)
                covertype::load_and_test(path, attack_file, feature_id, budget, n_inst, n_feats, epsilon);
            else if (dataset == DataSet::Mnist)
                mnist::load_and_test(path, attack_file, feature_id, budget, n_inst, n_feats, epsilon);
            return 0;
        }
        else if (opts.count("gain"))
        {
            auto mod = check_model(opts);
            auto [dataset, path] = parseTest(mod);
            if (dataset == DataSet::Credit)
                credit::put_gain_values(path);
            else if (dataset == DataSet::Har)
                har::put_gain_values(path);
            else if (dataset == DataSet::Covertype)
                covertype::put_gain_values(path);
            else if (dataset == DataSet::Mnist)
                mnist::put_gain_values(path);
            return 0;
        }
        else if (opts.count("classify"))
        {
			auto mod = check_model(opts);
            auto [dataset, path] = parseTest(mod);
            auto inst_vec = opts["classify"].as<std::vector<double>>();
            if (dataset == DataSet::Credit)
                credit::classify(path, inst_vec);
            else if (dataset == DataSet::Har)
                har::classify(path, inst_vec);
            else if (dataset == DataSet::Covertype)
                covertype::classify(path, inst_vec);
            else if (dataset == DataSet::Mnist)
                mnist::classify(path, inst_vec);
            return 0;
        }
        else if (opts.count("attack"))
        {
            if (attack_file.empty())
                throw std::invalid_argument("missing --attack-file");
            auto dataset = parseAttack(attack_file);
            auto att_inst = opts["attack"].as<std::vector<double>>();
            if (dataset == DataSet::Credit)
                credit::attack_instance(attack_file, att_inst, feature_id, budget, cost, epsilon);
            else if (dataset == DataSet::Har)
                har::attack_instance(attack_file, att_inst, feature_id, budget, cost, epsilon);
            else if (dataset == DataSet::Covertype)
                covertype::attack_instance(attack_file, att_inst, feature_id, budget, cost, epsilon);
            return 0;
        }

        if (!opts.count("data"))
                throw std::invalid_argument("missing --data");
        auto dataset = opts["data"].as<std::string>();
        auto algostr = opts["algo"].as<std::string>();
        auto par = opts["par"].as<bool>();
        auto verb = opts["verbosity"].as<int>();

        if (par)
            fmt::print("number of hardware threads = {}\n", std::thread::hardware_concurrency());

        toLower(dataset); 

        if (dataset == "credit")
        {
            credit::set_verbosity(verb);
            if (opts.count("batch"))
            {
                auto batch_file = opts["batch"].as<std::string>();
                credit::batch_train_and_save(opts, batch_file);
            }
            else if (opts.count("sweep"))
            {
                credit::argument_sweep(opts);
            }
            else
                credit::train_and_save(opts);
        }
        else if (dataset == "har")
        {
            har::set_verbosity(verb);
            if (opts.count("batch"))
            {
                auto batch_file = opts["batch"].as<std::string>();
                har::batch_train_and_save(opts, batch_file);
            }
            else if (opts.count("sweep"))
            {
                har::argument_sweep(opts);
            }
            else
                har::train_and_save(opts);
        }
        else if (dataset == "covertype")
        {
            covertype::set_verbosity(verb);
            if (opts.count("batch"))
            {
                auto batch_file = opts["batch"].as<std::string>();
                covertype::batch_train_and_save(opts, batch_file);
            }
            else if (opts.count("sweep"))
            {
                covertype::argument_sweep(opts);
            }
            else
                covertype::train_and_save(opts);
        }
        else if (dataset == "mnist")
        {
            mnist::set_verbosity(verb);
            if (opts.count("sweep"))
            {
                mnist::argument_sweep(opts);
            }
            else
                mnist::train_and_save(opts);
        }
        else
            Util::die("--data must be one of following: credit, har, covertype, mnist");
        
    }
    catch(const std::invalid_argument& e)
    {
        Util::die("{}\nsee ./treant -h for usage.", e.what());
    }
    catch(const std::exception& e)
    {
        Util::die("{}", e.what());
    }
    
    return 0;
}
