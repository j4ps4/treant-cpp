#pragma once
#include <array>
#include <string_view>
#include <stdexcept>
#include <algorithm>

template <typename Key, typename Value, std::size_t Size>
struct Map {
  std::array<std::pair<Key, Value>, Size> data;

  [[nodiscard]] constexpr Value at(const Key &key) const {
    const auto itr =
        std::find_if(begin(data), end(data),
                     [&key](const auto &v) { return v.first == key; });
    if (itr != end(data)) {
      return itr->second;
    } else {
      throw std::range_error("Not Found");
    }
  }

};


using namespace std::literals::string_view_literals;
static constexpr std::array<std::pair<std::string_view, size_t>, 24> column_map{
    {{"LIMIT_BAL"sv,0},
    {"SEX"sv,1},
    {"EDUCATION"sv,2},
    {"MARRIAGE"sv,3},
    {"AGE"sv,4},
    {"PAY_0"sv,5},
    {"PAY_2"sv,6},
    {"PAY_3"sv,7},
    {"PAY_4"sv,8},
    {"PAY_5"sv,9},
    {"PAY_6"sv,10},
    {"BILL_AMT1"sv,11},
    {"BILL_AMT2"sv,12},
    {"BILL_AMT3"sv,13},
    {"BILL_AMT4"sv,14},
    {"BILL_AMT5"sv,15},
    {"BILL_AMT6"sv,16},
    {"PAY_AMT1"sv,17},
    {"PAY_AMT2"sv,18},
    {"PAY_AMT3"sv,19},
    {"PAY_AMT4"sv,20},
    {"PAY_AMT5"sv,21},
    {"PAY_AMT6"sv,22},
    {"default.payment.next.month"sv,23}}};


static inline size_t lookup(const std::string_view sv) {

  static constexpr auto map =
      Map<std::string_view, size_t, column_map.size()>{{column_map}};

  return map.at(sv);
}

