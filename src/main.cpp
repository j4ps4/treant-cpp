#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <chrono>

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

std::string pretty_timediff(double diff)
{
    int hours = std::floor(diff / (60.0*60.0));
    double hsm = hours*60*60;
    double secs1 = diff-hsm;
    int mins = std::floor(secs1 / 60.0);
    double minss = mins*60;
    double secs2 = secs1-minss;
    return fmt::format("{}h {}m {:.2f}s", hours, mins, secs2);
}

using namespace std::chrono;
#define TIME duration_cast<duration<double>>(high_resolution_clock::now() - now).count()

int main(int argc, const char** argv)
{
    auto m_df = credit::read_train();
    if (m_df.has_error())
        Util::die("{}", m_df.error());
    auto& df_tupl = m_df.value();
    auto& X = std::get<0>(df_tupl);
    auto& Y = std::get<1>(df_tupl);

    auto m_test = credit::read_test();
    if (m_test.has_error())
        Util::die("{}", m_test.error());
    auto& test_tupl = m_test.value();
    auto& X_test = std::get<0>(test_tupl);
    auto& Y_test = std::get<1>(test_tupl);

    // auto df2 = df.slice(0,10);
    // for (const auto& row : df2)
    //     fmt::print("{}\n", row);
    auto m_atkr = credit::new_Attacker(50, X);
    if (m_atkr.has_error())
        Util::die("{}", m_atkr.error());
    fmt::print("X: a dataframe of size ({}x{})\n", X.rows(), X.cols());
    fmt::print("Y: a dataframe of size ({}x{})\n", Y.rows(), Y.cols());
    // for (int i = 0; i < 10; i++)
    // {
    //     std::cout << "X: " << X.row(i) << '\n';
    //     std::cout << "Y: " << Y.row(i) << '\n';
    // }
    // std::cout << Y.colwise().mean() << std::endl;
    high_resolution_clock::time_point now = high_resolution_clock::now();
    auto tree = credit::new_RDT({.id = 0, .attacker = std::move(m_atkr.value()),
        .fun = SplitFunction::LogLoss, .icml2019 = true, .max_depth = 8, 
        .min_instances_per_node = 20, .affine = true});
    tree.fit(X, Y);
    double linear_time = TIME;
    fmt::print("time elapsed: ");
    fmt::print(fg(fmt::color::yellow_green), "{}\n", pretty_timediff(linear_time));
    auto Y_pred = tree.predict(X_test);
    fmt::print("Y_test vs. Y_pred:\n");
    for (int i = 0; i < 5; i++)
    {
        std::cout << Y_test.row(i) << " <-> " << Y_pred.row(i) << "\n";
    }
    fmt::print("test error: {}\n", tree.classification_error(Y_test, Y_pred));
}
