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
#include "util.h"

TrainingAlgo parse_algo(const std::string& s)
{
    if (s == "robust")
        return TrainingAlgo::Robust;
    else if (s == "icml2019")
        return TrainingAlgo::Icml2019;
    else if (s == "standard")
        return TrainingAlgo::Standard;
    else
        Util::die("invalid algorithm");
}

enum class DataSet
{
    Credit, Har
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
        ("gain", "compute feature importance values for a trained model",
        cxxopts::value<bool>()->default_value("false"))
        ("classify", "classify given instance, comma separated list of values",
        cxxopts::value<std::vector<double>>())
        ("attack", "attack given instance, comma separated list of values",
        cxxopts::value<std::vector<double>>())
        ("batch", "train a number of trees in batch mode, using given batch file",
        cxxopts::value<std::string>())

        ("h,help", "print usage");

    options.add_options("file")
        ("data", "dataset to use",
        cxxopts::value<std::string>())
        ("O,output", "model output name",
        cxxopts::value<std::string>()->default_value(""))
        ("M,model", "trained model on which to operate",
        cxxopts::value<std::string>())
        ("attack_file", "json file describing the attacks",
        cxxopts::value<std::string>()->default_value(""));

    options.add_options("tree")
        ("algo", "training algorithm: robust, icml2019, standard",
        cxxopts::value<std::string>()->default_value("robust"))
        ("B,budget", "maximum budget of attacker",
        cxxopts::value<int>()->default_value("5"))
        ("maxiter", "maximum nlopt iterations",
        cxxopts::value<int>()->default_value("100"))
        ("D,maxdepth", "maximum depth of tree",
        cxxopts::value<size_t>()->default_value("8"))
        ("N,n_trees", "amount of trees in ensemble",
        cxxopts::value<size_t>()->default_value("1"))
        ("n_inst", "number of instances to use in training",
        cxxopts::value<int>()->default_value("-1"))
        ("bootstrap_instances", "bootstrap instances when training an ensemble",
        cxxopts::value<bool>()->default_value("false"))
        ("bootstrap_features", "bootstrap features when training an ensemble",
        cxxopts::value<bool>()->default_value("false"))
        ("replace_instances", "whether the random sampling of instances should be with replacement",
        cxxopts::value<bool>()->default_value("false"))
        ("replace_features", "whether the random sampling of features should be with replacement",
        cxxopts::value<bool>()->default_value("false"))
        ("max_instances", "proportion of instances sampled",
        cxxopts::value<double>()->default_value("1.0"))
        ("max_features", "proportion of features sampled",
        cxxopts::value<double>()->default_value("1.0"))
        ("par", "train single tree in parallel mode",
        cxxopts::value<bool>()->default_value("false"))
        ("feature_blacklist", "features to ignore when considering a split",
        cxxopts::value<std::vector<size_t>>()->default_value(""))
        ("feature_ids", "features to use in constructing ID# rules",
        cxxopts::value<std::vector<size_t>>()->default_value(""));

    options.add_options("internal")
        ("cost", "starting cost",
        cxxopts::value<int>()->default_value("0"))
        ("par_par", "internal parallel mode argument",
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

        auto feature_id_vec = opts["feature_ids"].as<std::vector<size_t>>();
        std::set<size_t> feature_id;
        for (auto f : feature_id_vec)
            feature_id.insert(f);

        auto attack_file = opts["attack_file"].as<std::string>();
        auto budget = opts["budget"].as<int>();
        auto cost = opts["cost"].as<int>();
        if (opts.count("test"))
        {
            auto mod = check_model(opts);
            auto [dataset, path] = parseTest(mod);
            if (dataset == DataSet::Credit)
                credit::load_and_test(path, attack_file, feature_id, budget);
            else if (dataset == DataSet::Har)
                har::load_and_test(path, attack_file, feature_id, budget);
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
            return 0;
        }
        else if (opts.count("attack"))
        {
            if (attack_file.empty())
                throw std::invalid_argument("missing --attack_file");
            auto dataset = parseAttack(attack_file);
            auto att_inst = opts["attack"].as<std::vector<double>>();
            if (dataset == DataSet::Credit)
                credit::attack_instance(attack_file, att_inst, feature_id, budget, cost);
            else if (dataset == DataSet::Har)
                har::attack_instance(attack_file, att_inst, feature_id, budget, cost);
            return 0;
        }

        if (!opts.count("data"))
                throw std::invalid_argument("missing --data");
        auto dataset = opts["data"].as<std::string>();
        auto algostr = opts["algo"].as<std::string>();
        auto outputstr = opts["output"].as<std::string>();
        auto maxiter = opts["maxiter"].as<int>();
        auto n_trees = opts["n_trees"].as<size_t>();
        auto maxdepth = opts["maxdepth"].as<size_t>();
        auto n_inst = opts["n_inst"].as<int>();
        auto par_par = opts["par_par"].as<double>();
        auto par = opts["par"].as<bool>();
        auto bootstrap_samples = opts["bootstrap_instances"].as<bool>();
        auto bootstrap_features = opts["bootstrap_features"].as<bool>();
        auto replace_samples = opts["replace_instances"].as<bool>();
        auto replace_features = opts["replace_features"].as<bool>();
        auto max_samples = opts["max_instances"].as<double>();
        auto max_features = opts["max_features"].as<double>();
        auto feature_bl_vec = opts["feature_blacklist"].as<std::vector<size_t>>();
        std::set<size_t> feature_bl;
        for (auto f : feature_bl_vec)
            feature_bl.insert(f);

        toLower(dataset); 
        toLower(algostr); 
        auto algo = parse_algo(algostr);

        if (dataset == "credit")
        {
            if (opts.count("batch"))
            {
                auto batch_file = opts["batch"].as<std::string>();
                credit::batch_train_and_save(
                    {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                                .id=0, .max_depth=maxdepth, .min_instances_per_node=20, .affine=true,
                                .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                                .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                                .replace_features=replace_features, .max_samples=max_samples, 
                                .max_features=max_features},
                    .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                    .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
                    .n_trees=n_trees}, batch_file
                );
            }
            credit::train_and_save(
                {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                            .id=0, .max_depth=maxdepth, .min_instances_per_node=20, .affine=true,
                            .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                            .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                            .replace_features=replace_features, .max_samples=max_samples, 
                            .max_features=max_features},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
                .n_trees=n_trees}
            );
        }
        else if (dataset == "har")
            har::train_and_save( 
                 {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                            .id=0, .max_depth=maxdepth, .min_instances_per_node=20, .affine=true,
                            .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                            .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                            .replace_features=replace_features, .max_samples=max_samples, 
                            .max_features=max_features},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter,
                .n_trees=n_trees}
            );
        else
            Util::die("--data must be one of following: credit, har");
        
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
