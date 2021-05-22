#include <fstream>
#include <sstream>
#include <iostream>
#include <json11.hpp>

#define CATCH_CONFIG_MAIN // This tells the catch header to generate a main
#include <catch2/catch.hpp>

/** Read file into string. */
inline std::string slurp (const std::string& path) {
  std::ostringstream buf; 
  std::ifstream input (path.c_str()); 
  buf << input.rdbuf(); 
  return buf.str();
}


TEST_CASE("json parsing", "[json]") {
    std::string err;
    auto json_str = slurp("../attacks.json");
    auto json = json11::Json::parse(json_str, err);
    REQUIRE(err.empty());
    SECTION("first feature is PAY_0") {
      auto ft = json["attacks"][0].object_items().cbegin()->first;
      REQUIRE(ft == "PAY_0");
    }
}