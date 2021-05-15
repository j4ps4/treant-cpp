#include <iostream>
#include <iomanip>
#include <utility>
#include <algorithm>
#include <numeric>

#include "def.h"
#include "util.h"
#include "df_util.h"

namespace {

const size_t MAX_W = 30UL;

// from Stackoverflow
// size_t utf8_length(const std::string& s) {
//     size_t len = 0;
//     for (auto c : s)
//         len += (c & 0xc0) != 0x80;
//     return len;
// }

// template<typename T>
// std::ostream& put_width(std::ostream& os, const T& val, size_t w)
// {
//     os << std::setw(w) << std::setfill(' ') << val;
//     return os;
// }

// template<>
// std::ostream& put_width<std::string>(std::ostream& os, const std::string& val, size_t w)
// {
//     std::string clamped = val.substr(0, MAX_W);
//     size_t diff = w - utf8_length(clamped);
//     for (size_t i = 0; i < diff; i++)
//         os << ' ';
//     os << clamped;
//     return os;
// }

// template<typename T>
// constexpr size_t width_helper(const T& val)
// {
//     return 4;
// }

// template<>
// size_t width_helper<std::string>(const std::string& val)
// {
//     return utf8_length(val);
// }

// template<>
// constexpr size_t width_helper<Time>(const Time& val)
// {
//     return 11;
// }

// template<>
// constexpr size_t width_helper<Sex>(const Sex& val)
// {
//     return 6;
// }

// template<>
// constexpr size_t width_helper<Distance>(const Distance& val)
// {
//     return 6;
// }

// template<typename T1>
// void col_width(const std::vector<const void*>& ptrs, std::vector<size_t>& ws, size_t col_idx)
// {
//     const auto& vec = *((const std::vector<T1>*)ptrs[col_idx]);
//     auto lengest = std::max_element(vec.cbegin(), vec.cend(),
//         [&](const auto& s1, const auto& s2)->bool{return width_helper(s1) < width_helper(s2);});
//     size_t l = std::clamp(width_helper(*lengest), 3UL, MAX_W);
//     ws.push_back(l);
// }

// template<typename T1, typename T2, typename... Ts>
// void col_width(const std::vector<const void*>& ptrs, std::vector<size_t>& ws, size_t col_idx)
// {
//     const auto& vec = *((const std::vector<T1>*)ptrs[col_idx]);
//     auto lengest = std::max_element(vec.cbegin(), vec.cend(),
//         [&](const auto& s1, const auto& s2)->bool{return width_helper(s1) < width_helper(s2);});
//     size_t l = std::clamp(width_helper(*lengest), 3UL, MAX_W);
//     ws.push_back(l);
//     col_width<T2,Ts...>(ptrs, ws, col_idx+1);
// }

void print_row(const std::vector<const void*>& ptrs,
               const std::map<size_t, std::pair<std::string, DataType>>& colmap,
               size_t row_idx)
{
    for (auto& [idx, pair] : colmap)
    {
        auto& colname = pair.first;
        auto& dtype = pair.second;
        switch(dtype)
        {
            case DataType::String:
            {
                const auto& vec = *((std::vector<std::string>*)ptrs[idx]);
                std::cout << vec[row_idx] << ',';
                break;
            }
            case DataType::Int:
            {
                const auto& vec = *((std::vector<int>*)ptrs[idx]);
                std::cout << vec[row_idx] << ',';
                break;
            }
            case DataType::UInt:
            {
                const auto& vec = *((std::vector<unsigned int>*)ptrs[idx]);
                std::cout << vec[row_idx] << ',';
                break;
            }
            case DataType::Double:
            {
                const auto& vec = *((std::vector<double>*)ptrs[idx]);
                std::cout << std::setprecision(12) << vec[row_idx] << ',';
                break;
            }
            case DataType::Float:
            {
                const auto& vec = *((std::vector<float>*)ptrs[idx]);
                std::cout << vec[row_idx] << ',';
                break;
            }
            case DataType::Char:
            {
                const auto& vec = *((std::vector<signed char>*)ptrs[idx]);
                std::cout << vec[row_idx] << ',';
                break;
            }
            case DataType::UChar:
            {
                const auto& vec = *((std::vector<unsigned char>*)ptrs[idx]);
                std::cout << vec[row_idx] << ',';
                break;
            }
            case DataType::Bool:
            {
                const auto& vec = *((std::vector<bool>*)ptrs[idx]);
                std::cout << vec[row_idx] << ',';
                break;
            }
            default:
            {
                Util::die("Invalid datatype for column\n");
            }
        }
    }
    std::cout << '\n';
}

}

namespace df
{

void load_addresses(const DF& df,
                    std::vector<const void*>& ptrs)
{
    for (auto& [idx, pair] : df.colmap_)
    {
        auto& colname = pair.first;
        auto& dtype = pair.second;
        switch(dtype)
        {
            case DataType::String:
            {
                ptrs.push_back(&(df.get_column<std::string>(colname.c_str())));
                break;
            }
            case DataType::Int:
            {
                ptrs.push_back(&(df.get_column<int>(colname.c_str())));
                break;
            }
            case DataType::UInt:
            {
                ptrs.push_back(&(df.get_column<unsigned int>(colname.c_str())));
                break;
            }
            case DataType::Double:
            {
                ptrs.push_back(&(df.get_column<double>(colname.c_str())));
                break;
            }
            case DataType::Float:
            {
                ptrs.push_back(&(df.get_column<float>(colname.c_str())));
                break;
            }
            case DataType::Char:
            {
                ptrs.push_back(&(df.get_column<signed char>(colname.c_str())));
                break;
            }
            case DataType::UChar:
            {
                ptrs.push_back(&(df.get_column<unsigned char>(colname.c_str())));
                break;
            }
            case DataType::Bool:
            {
                ptrs.push_back(&(df.get_column<bool>(colname.c_str())));
                break;
            }
            default:
            {
                Util::die("Invalid datatype for column\n");
            }
        }
    }
}

void print(const DF& df)
{
    // load the addresses of columns
    std::vector<const void*> ptrs;
    load_addresses(df, ptrs);

    // figure out column widths
    // std::vector<size_t> ws;
    // col_width<Ts...>(ptrs, ws, 0);

    // Print column names
    // for (size_t i = 0; i < I; i++)
    // {
    //     std::string col = col_names[i];
    //     if (ws[i] < col.size())
    //         col = col.substr(0, ws[i]);
    //     std::cout << std::setw(ws[i]) << std::setfill(' ') << col << ",";
    // }
    for (size_t i = 0; i < ptrs.size(); i++)
    {
        std::string col = df.colmap_.at(i).first;
        std::cout << col << ",";
    }
    std::cout << std::endl;
    // const size_t sum = std::accumulate(ws.begin(), ws.end(), 0);
    // for (size_t i = 0; i < sum+I; i++)
    //     std::cout << "-";
    // std::cout << std::endl;

    const auto [h, w] = df.shape();
    for (size_t r = 0; r < h; r++)
        print_row(ptrs, df.colmap_, r);
}

}