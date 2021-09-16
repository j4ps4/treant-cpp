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
        ("attack-file", "json file describing the attacks",
        cxxopts::value<std::string>()->default_value(""));

    options.add_options("tree")
        ("algo", "training algorithm: robust (TREANT), icml2019 (Chen et al.), standard",
        cxxopts::value<std::string>()->default_value("robust"))
        ("B,budget", "maximum budget of attacker",
        cxxopts::value<int>()->default_value("5"))
        ("maxiter", "maximum nlopt iterations",
        cxxopts::value<int>()->default_value("100"))
        ("epsilon", "radius of the l-inf ball (Chen et al.)",
        cxxopts::value<double>()->default_value("0.3"))
        ("D,maxdepth", "maximum depth of tree",
        cxxopts::value<std::vector<size_t>>()->default_value("8"))
        ("min-instances-per-node", "minimum instances per leaf node",
        cxxopts::value<std::vector<size_t>>()->default_value("20"))
        ("N,n-trees", "amount of trees in ensemble",
        cxxopts::value<size_t>()->default_value("1"))
        ("n-inst", "number of instances to use in training",
        cxxopts::value<int>()->default_value("-1"))
        ("sample-instances", "sample instances when training an ensemble",
        cxxopts::value<bool>()->default_value("false"))
        ("sample-features", "sample features when training an ensemble",
        cxxopts::value<bool>()->default_value("false"))
        ("replace-instances", "whether the random sampling of instances should be with replacement",
        cxxopts::value<bool>()->default_value("false"))
        ("max-instances", "proportion of instances sampled",
        cxxopts::value<double>()->default_value("1.0"))
        ("max-features", "proportion of features sampled",
        cxxopts::value<double>()->default_value("1.0"))
        ("affine", "don't consider the same features again when growing tree downward",
        cxxopts::value<std::vector<bool>>()->default_value("true"))
        ("par", "train single tree in parallel mode",
        cxxopts::value<bool>()->default_value("false"))
        ("feature-blacklist", "features to ignore when considering a split",
        cxxopts::value<std::vector<size_t>>()->default_value(""))
        ("feature-ids", "features to use in ID# rules, and Chen et al.",
        cxxopts::value<std::vector<size_t>>()->default_value(""));

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
            else if (dataset == DataSet::Covertype)
                covertype::load_and_test(path, attack_file, feature_id, budget);
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
            return 0;
        }
        else if (opts.count("attack"))
        {
            if (attack_file.empty())
                throw std::invalid_argument("missing --attack-file");
            auto dataset = parseAttack(attack_file);
            auto att_inst = opts["attack"].as<std::vector<double>>();
            if (dataset == DataSet::Credit)
                credit::attack_instance(attack_file, att_inst, feature_id, budget, cost);
            else if (dataset == DataSet::Har)
                har::attack_instance(attack_file, att_inst, feature_id, budget, cost);
            else if (dataset == DataSet::Covertype)
                covertype::attack_instance(attack_file, att_inst, feature_id, budget, cost);
            return 0;
        }

        if (!opts.count("data"))
                throw std::invalid_argument("missing --data");
        auto dataset = opts["data"].as<std::string>();
        auto algostr = opts["algo"].as<std::string>();
        auto outputstr = opts["output"].as<std::string>();
        auto maxiter = opts["maxiter"].as<int>();
        auto epsilon = opts["epsilon"].as<double>();
        auto n_trees = opts["n-trees"].as<size_t>();
        auto maxdepth_v = opts["maxdepth"].as<std::vector<size_t>>();
        auto maxdepth = maxdepth_v.front();
        auto min_inst_v = opts["min-instances-per-node"].as<std::vector<size_t>>();
        auto min_inst = min_inst_v.front();
        auto n_inst = opts["n-inst"].as<int>();
        auto par_par = opts["par-par"].as<double>();
        auto affine_v = opts["affine"].as<std::vector<bool>>();
        auto affine = affine_v.front();
        auto par = opts["par"].as<bool>();
        auto bootstrap_samples = opts["sample-instances"].as<bool>();
        auto bootstrap_features = opts["sample-features"].as<bool>();
        auto replace_samples = opts["replace-instances"].as<bool>();
        auto replace_features = opts["replace-features"].as<bool>();
        auto max_samples = opts["max-instances"].as<double>();
        auto max_features = opts["max-features"].as<double>();
        auto feature_bl_vec = opts["feature-blacklist"].as<std::vector<size_t>>();
        std::set<size_t> feature_bl;
        for (auto f : feature_bl_vec)
            feature_bl.insert(f);
        auto verb = opts["verbosity"].as<int>();

        toLower(dataset); 
        toLower(algostr); 
        auto algo = parse_algo(algostr);

        if (dataset == "credit")
        {
            credit::set_verbosity(verb);
            if (opts.count("batch"))
            {
                auto batch_file = opts["batch"].as<std::string>();
                credit::batch_train_and_save(
                    {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                                .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                                .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                                .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                                .replace_features=replace_features, .max_samples=max_samples, 
                                .max_features=max_features},
                    .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                    .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
                    .n_trees=n_trees, .epsilon=epsilon}, batch_file
                );
            }
            else if (opts.count("cross-val"))
            {
                auto N_folds = opts["cross-val"].as<size_t>();
                credit::cross_val_and_save(
                    {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                                .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                                .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                                .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                                .replace_features=replace_features, .max_samples=max_samples, 
                                .max_features=max_features},
                    .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                    .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
                    .n_trees=n_trees, .epsilon=epsilon},
                    {.N_folds=N_folds, .maxdepth=maxdepth_v, .min_inst=min_inst_v, .affine=affine_v}
                );
            }
            credit::train_and_save(
                {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                            .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                            .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                            .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                            .replace_features=replace_features, .max_samples=max_samples, 
                            .max_features=max_features},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
                .n_trees=n_trees, .epsilon=epsilon}
            );
        }
        else if (dataset == "har")
        {
            har::set_verbosity(verb);
            if (opts.count("batch"))
            {
                auto batch_file = opts["batch"].as<std::string>();
                har::batch_train_and_save(
                    {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                                .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                                .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                                .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                                .replace_features=replace_features, .max_samples=max_samples, 
                                .max_features=max_features},
                    .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                    .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
                    .n_trees=n_trees, .epsilon=epsilon}, batch_file
                );
            }
            har::train_and_save( 
                 {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                            .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                            .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                            .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                            .replace_features=replace_features, .max_samples=max_samples, 
                            .max_features=max_features},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter,
                .n_trees=n_trees, .epsilon=epsilon}
            );
        }
        else if (dataset == "covertype")
        {
            covertype::set_verbosity(verb);
            if (opts.count("batch"))
            {
                auto batch_file = opts["batch"].as<std::string>();
                covertype::batch_train_and_save(
                    {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                                .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                                .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                                .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                                .replace_features=replace_features, .max_samples=max_samples, 
                                .max_features=max_features},
                    .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                    .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
                    .n_trees=n_trees, .epsilon=epsilon}, batch_file
                );
            }
            covertype::train_and_save( 
                 {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                            .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                            .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                            .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                            .replace_features=replace_features, .max_samples=max_samples, 
                            .max_features=max_features},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter,
                .n_trees=n_trees, .epsilon=epsilon}
            );
        }
        else if (dataset == "mnist")
        {
            mnist::set_verbosity(verb);
            // if (opts.count("batch"))
            // {
            //     auto batch_file = opts["batch"].as<std::string>();
            //     covertype::batch_train_and_save(
            //         {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
            //                     .id=0, .max_depth=maxdepth, .min_instances_per_node=20, .affine=affine,
            //                     .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
            //                     .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
            //                     .replace_features=replace_features, .max_samples=max_samples, 
            //                     .max_features=max_features},
            //         .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
            //         .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter, 
            //         .n_trees=n_trees}, batch_file
            //     );
            // }
            mnist::train_and_save( 
                 {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                            .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                            .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                            .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                            .replace_features=replace_features, .max_samples=max_samples, 
                            .max_features=max_features},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget, .feature_ids = feature_id,
                .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter,
                .n_trees=n_trees, .epsilon=epsilon}
            );
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
