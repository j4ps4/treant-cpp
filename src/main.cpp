#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>

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
    if (auto err = std::get_if<std::string>(&res))
        Util::die("{}", *err);
    auto& df = std::get<DF>(res);
    DF df2 =
        df.get_data_by_idx(hmdf::Index2D<IdxT> {0, 10});
    df::print(df2);
    size_t cidx = 10;
    fmt::print("shape of df2: {}\n", df2.shape());
    auto view = df2.get_view_by_idx(cidx);
    auto& val = view.get<signed char>(23);
    fmt::print("value at index {}: {}\n", cidx, val);
    fmt::print("shape of df: {}\n", df.shape());
    auto res2 = load_attack_rules("attacks.json", df.get_colmap());
    if (auto err = std::get_if<std::string>(&res2))
        Util::die("{}", *err);
    auto& attkl = std::get<AttkList>(res2);
    for (auto& attk : attkl)
        fmt::print("{}\n", attk.debug_str());
}