#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <charconv>

#include <fmt/core.h>

#include "def.h"
#include "util.h"
#include "df_util.h"
#include "df_read.h"

namespace {

constexpr int BS = 1048576; // 1 GB

ColMap parseColumns(const std::string& s)
{
    auto splitByColon = [](const std::string& s)->std::pair<std::string,std::string>{
        auto pos = s.find(':');
        if (pos != std::string::npos)
            return {s.substr(0, pos), s.substr(pos+1)};
        else
            return {s, ""};
    };
    ColMap out;
    size_t idx = 0;
    std::size_t pos = 0;
    std::size_t pos1 = std::string::npos;
    do {
        pos1 = s.find(',', pos);
        if (pos1 != std::string::npos)
        {
            auto frag = s.substr(pos, pos1-pos);
            auto [before, after] = splitByColon(frag);
            out[idx] = {before, Util::readDt(after)};
            pos = pos1+1;
            idx++;
        }
    } while (pos1 != std::string::npos && pos != 0);
    auto frag = s.substr(pos);
    auto [before, after] = splitByColon(frag);
    out[idx] = {before, Util::readDt(after)};
    return out;
}

}

namespace df {

std::variant<DF, std::string> read_bz2(const char* fn)
{
    FILE*   f;
    BZFILE* b;
    int     nBuf;
    char    buf[BS];
    int     bzerror;
    int     nWritten;

    f = fopen(fn, "r");
    if (!f) {
        return fmt::format("cannot open {}: {}", fn, strerror(errno));
    }
    b = BZ2_bzReadOpen ( &bzerror, f, 0, 0, NULL, 0);
    if ( bzerror != BZ_OK ) {
        BZ2_bzReadClose ( &bzerror, b );
        return fmt::format("bzerror: {}", bzerror);
    }

    bzerror = BZ_OK;
    while ( bzerror == BZ_OK ) {
        nBuf = BZ2_bzRead ( &bzerror, b, buf, BS);
        if (bzerror == BZ_OK) { // returns BZ_STREAM_END when file ends
            return "buff too small";
        }
    }
    if (bzerror != BZ_STREAM_END) {
        BZ2_bzReadClose( &bzerror, b );
        return fmt::format("bzerror: {}", bzerror);
    } else {
        BZ2_bzReadClose( &bzerror, b );
    }
    fclose(f);
    auto fstlne = std::find(buf, buf+nBuf, '\n');
    auto fstln = std::string(buf, fstlne);
    auto colmap = parseColumns(fstln);
    DataFrame df;

    const auto rows = std::count(buf, buf+nBuf, '\n') - 1;
    std::vector<IdxT> idx(rows);
    std::iota(idx.begin(), idx.end(), 0);
    df.load_index(std::move(idx));
    for (auto& [idx, pair] : colmap)
    {
        const auto& colname = pair.first;
        const auto dtype = pair.second;
        switch(dtype)
        {
            case DataType::Int:
            {
                std::vector<int> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::UInt:
            {
                std::vector<unsigned int> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::Double:
            {
                std::vector<double> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::Float:
            {
                std::vector<float> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::Char:
            {
                std::vector<signed char> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::UChar:
            {
                std::vector<unsigned char> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::Bool:
            {
                std::vector<bool> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            default:
            {
                return "invalid datatype!";
            }
        }
    }
    DF out(std::move(df), std::move(colmap));
    std::vector<const void*> ptrs;
    df::load_addresses(out, ptrs);
    auto loc = fstlne+1;
    const auto chars = {',', '\n'};
    const auto ncols = colmap.size();
    for (long r = 0; r < rows; r++)
    {
        for (auto& [idx, pair] : colmap)
        {
            const auto& colname = pair.first;
            const auto dtype = pair.second;
            auto loc1 = std::find_first_of(loc, buf+nBuf, chars.begin(), chars.end());
            if (*loc1 == '\n' && idx != ncols - 1)
            {
                return fmt::format("malformed line at {}", r + 1);
            }
            // fmt::print("emplacing {}\n", std::string(loc,loc1));
            switch(dtype)
            {
                case DataType::Int:
                {
                    auto& vec = *((std::vector<int>*)ptrs[idx]);
                    int val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val);
                    break;
                }
                case DataType::UInt:
                {
                    auto& vec = *((std::vector<unsigned int>*)ptrs[idx]);
                    unsigned int val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val);
                    break;
                }
                case DataType::Double:
                {
                    auto& vec = *((std::vector<double>*)ptrs[idx]);
                    double val = strtod(loc, nullptr);
                    vec.emplace_back(val);
                    break;
                }
                case DataType::Float:
                {
                    auto& vec = *((std::vector<float>*)ptrs[idx]);
                    float val = strtof(loc, nullptr);
                    vec.emplace_back(val);
                    break;
                }
                case DataType::Char:
                {
                    auto& vec = *((std::vector<signed char>*)ptrs[idx]);
                    signed int val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(static_cast<signed char>(val));
                    break;
                }
                case DataType::UChar:
                {
                    auto& vec = *((std::vector<unsigned char>*)ptrs[idx]);
                    unsigned int val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(static_cast<unsigned char>(val));
                    break;
                }
                case DataType::Bool:
                {
                    auto& vec = *((std::vector<bool>*)ptrs[idx]);
                    int val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val > 0 ? true : false);
                    break;
                }
                default:
                {
                    return "invalid datatype!";
                }
            }
            loc = loc1 + 1;
        }
    }
    // for (const auto& col : colnames)
    //     fmt::print("{},", col);
    // fmt::print("\n");

    // for (int i = 0; i < 10; i++)
    // {
    //     for (const auto& col : colnames)
    //     {
    //         fmt::print("{}, ", df.get_column<std::string>(col.c_str())[i]);
    //     }
    //     fmt::print("\n");
    // }
    //fmt::print("{}\n", df.get_column<int>((std::size_t)1));
    // DataFrame df2 =
    //     df.get_data_by_idx<std::string, int>(hmdf::Index2D<IdxT> {0, 10});
    // auto& col = df2.get_column<std::string>((size_t)0);
    // fmt::print("{}\n", col);
    // df::print(df2, colmap);
    // df2.write<std::ostream, std::string, int>(std::cout, hmdf::io_format::csv2); 
    // fmt::print("df shape: ({}, {})\n", df.shape().first, df.shape().second);
    return out;
}

}