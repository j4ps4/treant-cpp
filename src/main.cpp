#include <fmt/core.h>
#include <cxxopts.hpp>
#include <stdio.h>
#include <ranges>
#include <cctype>

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
    else
        Util::die("invalid algorithm");
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
        ("data", "dataset to use",
        cxxopts::value<std::string>()->default_value("credit"))
        ("algo", "algorithm: robust, icml2019",
        cxxopts::value<std::string>()->default_value("robust"))
        ("budget", "maximum budget of attacker",
        cxxopts::value<int>()->default_value("50"))
        ("h,help", "print usage");
    auto opts = options.parse(argc, argv);
    if (opts.count("help"))
    {
        fmt::print("{}\n", options.help());
        exit(0);
    }
    auto dataset = opts["data"].as<std::string>();
    auto algostr = opts["algo"].as<std::string>();
    auto budget = opts["budget"].as<int>();

    toLower(dataset); 
    toLower(algostr); 
    auto algo = parse_algo(algostr);

    if (dataset == "credit")
        credit::train_and_test(SplitFunction::LogLoss, algo, 8, 20, budget, true);
    else if (dataset == "covertype")
        covertype::train_and_test(SplitFunction::LogLoss, algo, 8, 20, budget, true);
    else if (dataset == "har")
        har::train_and_test(SplitFunction::LogLoss, algo, 8, 20, budget, true);
    else
        Util::die("invalid dataset: {}", dataset);
    
    return 0;
}
