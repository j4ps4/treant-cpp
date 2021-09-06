#include <fmt/core.h>
#include <cxxopts.hpp>
#include <stdio.h>
#include <ranges>
#include <cctype>
#include <tuple>
#include <filesystem>

#include "dataset/Credit.h"
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
    const auto dirname = *beg;
    if (dirname == "credit")
        return {DataSet::Credit, p};
    else if (dirname == "har")
        return {DataSet::Har, p};
    else
        Util::die("model path is not valid");
}
 
template<typename Range>
void toLower(Range& rang)
{
    std::ranges::transform(rang.begin(), rang.end(), rang.begin(),
                   [](unsigned char c) -> unsigned char { return std::tolower(c); });
}

int main(int argc, char** argv)
{
    cxxopts::Options options("treant", "robust tree classifier");
    options.add_options()
        ("test", "perform testing on a trained model",
        cxxopts::value<bool>()->default_value("false"))
        ("gain", "compute feature importance values for a trained model",
        cxxopts::value<bool>()->default_value("false"))
        ("m,model", "model on which to operate",
        cxxopts::value<std::string>())
        ("classify", "instance to classify, comma separated list of values",
        cxxopts::value<std::vector<double>>())
        ("data", "dataset to use",
        cxxopts::value<std::string>()->default_value("credit"))
        ("algo", "algorithm: robust, icml2019, standard",
        cxxopts::value<std::string>()->default_value("robust"))
        ("attack_file", "json file describing the attacks",
        cxxopts::value<std::string>()->default_value(""))
        ("budget", "maximum budget of attacker",
        cxxopts::value<int>()->default_value("50"))
        ("maxiter", "maximum nlopt iterations",
        cxxopts::value<int>()->default_value("100"))
        ("maxdepth", "maximum depth of tree",
        cxxopts::value<size_t>()->default_value("8"))
        ("n_inst", "number of instances to use in training",
        cxxopts::value<int>()->default_value("-1"))
        ("feature_blacklist", "features to ignore when considering a split",
        cxxopts::value<std::vector<size_t>>()->default_value(""))
        ("h,help", "print usage");
    
    try
    {
        auto opts = options.parse(argc, argv);
        if (opts.count("help"))
        {
            fmt::print("{}\n", options.help());
            exit(0);
        }
        auto check_model = [](const auto& arg)->std::string{
            if (!arg.count("model"))
                throw std::invalid_argument("model parameter is missing");
            return arg["model"].template as<std::string>();
        };

        auto attack_file = opts["attack_file"].as<std::string>();
        if (opts.count("test"))
        {
            auto mod = check_model(opts);
            auto [dataset, path] = parseTest(mod);
            if (dataset == DataSet::Credit)
                credit::load_and_test(path, attack_file);
            else if (dataset == DataSet::Har)
                har::load_and_test(path, attack_file);
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

        auto dataset = opts["data"].as<std::string>();
        auto algostr = opts["algo"].as<std::string>();
        auto budget = opts["budget"].as<int>();
        auto maxiter = opts["maxiter"].as<int>();
        auto maxdepth = opts["maxdepth"].as<size_t>();
        auto n_inst = opts["n_inst"].as<int>();
        auto feature_bl_vec = opts["feature_blacklist"].as<std::vector<size_t>>();
        std::set<size_t> feature_bl;
        for (auto f : feature_bl_vec)
            feature_bl.insert(f);

        toLower(dataset); 
        toLower(algostr); 
        auto algo = parse_algo(algostr);

        if (dataset == "credit")
            credit::train_and_save(
                {.tree_args = {.id=0, .fun=SplitFunction::LogLoss, .algo=algo, .max_depth=maxdepth,
                            .min_instances_per_node=20, .maxiter=maxiter, .affine=true, .feature_bl=feature_bl},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget}
            );
        else if (dataset == "har")
            har::train_and_save(
                {.tree_args = {.id=0, .fun=SplitFunction::LogLoss, .algo=algo, .max_depth=maxdepth,
                            .min_instances_per_node=20, .maxiter=maxiter, .affine=true, .feature_bl=feature_bl},
                .attack_file = attack_file, .n_inst = n_inst, .budget = budget}
            );
        else
            Util::die("invalid dataset: {}", dataset);
        
    }
    catch(const std::exception& e)
    {
        Util::die(e.what());
    }
    return 0;
}
