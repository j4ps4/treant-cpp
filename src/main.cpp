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
        ("test", "perform testing on trained model",
        cxxopts::value<std::string>()->default_value(""))
        ("data", "dataset to use",
        cxxopts::value<std::string>()->default_value("credit"))
        ("algo", "algorithm: robust, icml2019, standard",
        cxxopts::value<std::string>()->default_value("robust"))
        ("budget", "maximum budget of attacker",
        cxxopts::value<int>()->default_value("50"))
        ("maxiter", "maximum nlopt iterations",
        cxxopts::value<int>()->default_value("100"))
        ("maxdepth", "maximum depth of tree",
        cxxopts::value<int>()->default_value("8"))
        ("n_inst", "number of instances to use in training",
        cxxopts::value<int>()->default_value("-1"))
        ("h,help", "print usage");
    auto opts = options.parse(argc, argv);
    if (opts.count("help"))
    {
        fmt::print("{}\n", options.help());
        exit(0);
    }

    if (opts.count("test"))
    {
        auto [dataset, path] = parseTest(opts["test"].as<std::string>());
        if (dataset == DataSet::Credit)
            credit::load_and_test(path);
        else if (dataset == DataSet::Har)
            har::load_and_test(path);
        return 0;
    }

    auto dataset = opts["data"].as<std::string>();
    auto algostr = opts["algo"].as<std::string>();
    auto budget = opts["budget"].as<int>();
    auto maxiter = opts["maxiter"].as<int>();
    auto maxdepth = opts["maxdepth"].as<int>();
    auto n_inst = opts["n_inst"].as<int>();

    toLower(dataset); 
    toLower(algostr); 
    auto algo = parse_algo(algostr);

    if (dataset == "credit")
        credit::train_and_test(SplitFunction::LogLoss, algo, maxdepth, 20, budget, true, n_inst);
    else if (dataset == "covertype")
        covertype::train_and_test(SplitFunction::LogLoss, algo, maxdepth, 20, budget, true);
    else if (dataset == "har")
        har::train_and_test(SplitFunction::LogLoss, algo, maxdepth, 20, budget, maxiter, true, n_inst);
    else
        Util::die("invalid dataset: {}", dataset);
    
    return 0;
}
