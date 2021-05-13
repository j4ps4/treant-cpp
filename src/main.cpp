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
        Util::die("usage\n");
    }

    auto fn = argv[1];
    auto res = df::read_bz2(fn);
    if (auto err = std::get_if<std::string>(&res))
        Util::die("{}\n", *err);
    auto& df = std::get<DataFrame>(res);
    df.write<std::ostream, std::string, int>(std::cout, hmdf::io_format::csv2); 
}