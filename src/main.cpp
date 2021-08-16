#include <fmt/core.h>
#include <cxxopts.hpp>
#include <stdio.h>

#include "Credit.h"
#include "Forest.h"
#include "util.h"


int main(int argc, char** argv)
{
    cxxopts::Options options("treant", "robust tree classifier");
    options.add_options()
        ("data", "dataset to use",
        cxxopts::value<std::string>()->default_value("credit"))
        ("h,help", "print usage");
    auto opts = options.parse(argc, argv);
    if (opts.count("help"))
    {
        fmt::print("{}\n", options.help());
        exit(0);
    }
    auto dataset = opts["data"].as<std::string>();

    if (dataset == "credit")
        credit::train_and_test(SplitFunction::LogLoss, TrainingAlgo::Icml2019, 8, 20, true);
    else if (dataset == "forest")
        forest::train_and_test(SplitFunction::LogLoss, TrainingAlgo::Icml2019, 8, 20, true);
    else
        Util::die("invalid dataset: {}", dataset);
    
    return 0;
}
