#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include "util.h"
#include "df_read.h"
#include "df_util.h"
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
    auto res2 = load_attack_rules("attacks.json", df.get_colmap());
    if (res2.has_error())
        Util::die("{}", res2.error());
    auto& attkl = res2.value();
    auto height = df.shape().first;
    int counter = 0;
    for (size_t i = 0; i < height; i++)
    {
        auto rw = df.get_row_view(i);
        std::for_each(attkl.cbegin(), attkl.cend(), [&](const auto& atk){
            if (atk.is_applicable(rw))
            {
                auto xprime = atk.apply(rw);
                fmt::print("original:\n");
                df::print(rw);
                fmt::print("attacking feature {}:\n", atk.get_target_feature());
                df::print(xprime);
                counter++;
            }
        });
        if (counter > 10)
            break;
    }

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
//     DF df2 =
//         df.get_data_by_idx(hmdf::Index2D<IdxT> {0, 10});
//     df::print(df2);
//     size_t cidx = 10;
//     fmt::print("shape of df2: {}\n", df2.shape());
//     auto view = df2.get_row_view(cidx);
//     auto val = view.get_as_i32(21);
//     fmt::print("value at index {}: {}\n", cidx, val);
//     fmt::print("shape of df: {}\n", df.shape());
//     auto res2 = load_attack_rules("attacks.json", df.get_colmap());
//     if (res2.has_error())
//         Util::die("{}", res2.error());
//     auto& attkl = res2.value();
//     for (auto& attk : attkl)
//         fmt::print("{}\n", attk.debug_str());
// }