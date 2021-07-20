#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include "Credit.h"
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

int main(int argc, const char** argv)
{
    auto m_df = credit::read_bz2();
    if (m_df.has_error())
        Util::die("{}", m_df.error());
    auto& df_tupl = m_df.value();
    auto& X = std::get<0>(df_tupl);
    auto& Y = std::get<1>(df_tupl);
    // auto df2 = df.slice(0,10);
    // for (const auto& row : df2)
    //     fmt::print("{}\n", row);
    auto m_atkr = credit::new_Attacker(50, X);
    if (m_atkr.has_error())
        Util::die("{}", m_atkr.error());
    fmt::print("X: a dataframe of size ({}x{})\n", X.rows(), X.cols());
    fmt::print("Y: a dataframe of size ({}x{})\n", Y.rows(), Y.cols());
    for (int i = 0; i < 10; i++)
    {
        std::cout << "X: " << X.row(i) << '\n';
        std::cout << "Y: " << Y.row(i) << '\n';
    }
    // std::cout << Y.colwise().mean() << std::endl;
    auto tree = credit::new_RDT({.id = 0, .attacker = std::move(m_atkr.value()),
        .fun = SplitFunction::LogLoss, .icml2019 = true, .max_depth = 8, 
        .min_instances_per_node = 20, .affine = true});
    tree.fit(X, Y);
}
