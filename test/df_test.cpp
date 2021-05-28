#include <vector>

#include "Credit.h"

#define CATCH_CONFIG_MAIN // This tells the catch header to generate a main
#include <catch2/catch.hpp>

TEST_CASE("DF tests", "[df]") {
    auto res = credit::read_bz2("../valid.csv.bz2");
    REQUIRE(!res.has_error());
    auto& df = res.value();
    SECTION("valid.csv has 6000 lines") {
        auto h = df.height();
        REQUIRE(h == 6000);
    }
    SECTION("extracting first column") {
        auto vec = df.at<double>("LIMIT_BAL");
        REQUIRE(vec.size() == 6000);
        REQUIRE(vec[0] == std::get<0>(df[0]));
    }
}