#include <fmt/core.h>
#include <cxxopts.hpp>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include "Credit.h"
#include "Forest.h"
#include "util.h"

// int main(int argc, const char** argv)
// {
//     if (argc < 2)
//     {
//         Util::die("usage");
//     }

//     auto fn = argv[1];
//     auto res = df::read_bz2(fn);
//     if (res.has_error())
//         Util::die("{}", res.error());
//     auto& df = res.value();
//     size_t ridx = 2;
//     DF df2;
//     df2.append_row(df.get_row(ridx));
//     df::print(df2);
//     fmt::print("index: {}\n", df2.get_index());
//     auto df20 = df2.get_row(0);
//     auto rw = df.get_row_view(ridx);
//     df::print(rw);
//     fmt::print("rw == df20: {}\n", df20 == rw);
//     auto rw2 = df2.get_row_view(0);
//     fmt::print("rw == rw2: {}\n", rw == rw2);
// }


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
        credit::train_and_test(SplitFunction::LogLoss, true, 8, 20, true);
    else if (dataset == "forest")
        forest::train_and_test(SplitFunction::LogLoss, true, 8, 20, true);
    else
        Util::die("invalid dataset: {}", dataset);
    
    return 0;
}
