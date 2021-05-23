#include <vector>

#include "DF.h"
#include "DF_read.h"

#define CATCH_CONFIG_MAIN // This tells the catch header to generate a main
#include <catch2/catch.hpp>

TEST_CASE("DF tests", "[df]") {
    auto res = df::read_bz2("../valid.csv.bz2");
    REQUIRE(!res.has_error());
    auto& df = res.value();
    size_t ridx = 2;
    DF df2;
    df2.append_row(df.get_row(ridx));
    auto df2_0 = df2.get_row(0);
    auto rw = df.get_row_view(ridx);
    SECTION("appended row is equal to original") {
        REQUIRE(df2_0 == rw);
    }
    SECTION("row view is equal to original") {
        auto rw2 = df2.get_row_view(0);
        REQUIRE(rw2 == rw);
    }
    SECTION("row view is equal to itself") {
        auto rw2 = df2.get_row_view(0);
        REQUIRE(rw2 == rw2);
        REQUIRE(rw == rw);
    }
    SECTION("different row view is not equal") {
        auto rw2 = df.get_row_view(1);
        REQUIRE(!(rw2 == rw));
        REQUIRE(!(df2_0 == rw2));
    }
}