#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include "util.h"
#include "DF_read.h"
#include "DF_util.h"
#include "AttackerRule.h"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        Util::die("usage");
    }

    auto fn = argv[1];
    auto res = df::read_bz2(fn);
    if (res.has_error())
        Util::die("{}", res.error());
    auto& df = res.value();
    size_t ridx = 2;
    DF df2;
    df2.append_row(df.get_row(ridx));
    df::print(df2);
    fmt::print("index: {}\n", df2.get_index());
    auto df20 = df2.get_row(0);
    auto rw = df.get_row_view(ridx);
    df::print(rw);
    fmt::print("rw == df20: {}\n", df20 == rw);
    auto rw2 = df2.get_row_view(0);
    fmt::print("rw == rw2: {}\n", rw == rw2);
}

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
//     auto res2 = load_attack_rules("attacks.json", df.get_colmap());
//     if (res2.has_error())
//         Util::die("{}", res2.error());
//     auto& attkl = res2.value();
//     auto height = df.shape().first;
//     int counter = 0;
//     for (size_t i = 0; i < height; i++)
//     {
//         auto rw = df.get_row_view(i);
//         std::for_each(attkl.cbegin(), attkl.cend(), [&](const auto& atk){
//             if (atk.is_applicable(rw))
//             {
//                 auto xprime = atk.apply(rw);
//                 fmt::print("original:\n");
//                 df::print(rw);
//                 fmt::print("attacking feature {}:\n", atk.get_target_feature());
//                 df::print(xprime);
//                 counter++;
//             }
//         });
//         if (counter > 10)
//             break;
//     }

// }
