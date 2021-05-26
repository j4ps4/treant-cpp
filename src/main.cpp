#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include "DF2/DF.h"
#include "DF2/DF_util.h"
#include "DF2/DF_read.h"
#include "util.h"
#include "AttackerRule.h"
#include "Attacker.h"

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

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        Util::die("usage");
    }

    auto fn = argv[1];
    auto res = df::read_bz2<CREDIT_TYPES>(fn);
    if (res.has_error())
        Util::die("{}", res.error());
    auto& df = res.value();
    std::cout << df.height() << std::endl;
    auto df2 = df.slice(0,10);
    // for (const auto& row : df2)
    //     std::cout << row << std::endl;
    auto res2 = load_attack_rules("attacks.json");
    auto& rulz = res2.value();
    Attacker<CREDIT_TYPES> atkr(rulz, 50);
    atkr.compute_attacks(df, "lol");
}
