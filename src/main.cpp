#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"
#include "df_read.h"
#include "df_util.h"

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
    fmt::print("shape of df: {}\n", df.shape());
}