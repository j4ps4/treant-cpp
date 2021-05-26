#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <charconv>
#include <stdint.h>

#include <fmt/core.h>

#include "DF.h"
#include "DF_util.h"
#include "DF_read.h"

namespace {

DataTypeWBool readDt(const std::string& s)
{
    if (s == "i32")
        return DataTypeWBool::Int;
    else if (s == "u32")
        return DataTypeWBool::UInt;
    else if (s == "f64")
        return DataTypeWBool::Double;
    else if (s == "f32")
        return DataTypeWBool::Float;
    else if (s == "i8")
        return DataTypeWBool::Char;
    else if (s == "u8")
        return DataTypeWBool::UChar;
    else if (s == "bool")
        return DataTypeWBool::Bool;
    else if (s.empty())
        return DataTypeWBool::Double;
    else
    {
        auto s1 = fmt::format("invalid datatype specifier: {}", s);
        throw std::runtime_error(s1);
    }
}

constexpr int BS = 1048576; // 1 GB

ColMapWBool parseColumns(const std::string& s)
{
    auto splitByColon = [](const std::string& s)->std::pair<std::string,std::string>{
        auto pos = s.find(':');
        if (pos != std::string::npos)
            return {s.substr(0, pos), s.substr(pos+1)};
        else
            return {s, ""};
    };
    ColMapWBool out;
    size_t idx = 0;
    std::size_t pos = 0;
    std::size_t pos1 = std::string::npos;
    do {
        pos1 = s.find(',', pos);
        if (pos1 != std::string::npos)
        {
            auto frag = s.substr(pos, pos1-pos);
            auto [before, after] = splitByColon(frag);
            out[idx] = {before, readDt(after)};
            pos = pos1+1;
            idx++;
        }
    } while (pos1 != std::string::npos && pos != 0);
    auto frag = s.substr(pos);
    auto [before, after] = splitByColon(frag);
    out[idx] = {before, readDt(after)};
    return out;
}

}

namespace df {

cpp::result<DF, std::string> read_bz2(const char* fn)
{
    FILE*   f;
    BZFILE* b;
    int     nBuf;
    char    buf[BS];
    int     bzerror;
    int     nWritten;

    f = fopen(fn, "r");
    if (!f) {
        return cpp::failure(fmt::format("cannot open {}: {}", fn, strerror(errno)));
    }
    b = BZ2_bzReadOpen ( &bzerror, f, 0, 0, NULL, 0);
    if ( bzerror != BZ_OK ) {
        BZ2_bzReadClose ( &bzerror, b );
        return cpp::failure(fmt::format("bzerror: {}", bzerror));
    }

    bzerror = BZ_OK;
    while ( bzerror == BZ_OK ) {
        nBuf = BZ2_bzRead ( &bzerror, b, buf, BS);
        if (bzerror == BZ_OK) { // returns BZ_STREAM_END when file ends
            return cpp::failure(std::string("buff too small"));
        }
    }
    if (bzerror != BZ_STREAM_END) {
        BZ2_bzReadClose( &bzerror, b );
        return cpp::failure(fmt::format("bzerror: {}", bzerror));
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
            case DataTypeWBool::Int:
            {
                std::vector<int32_t> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataTypeWBool::UInt:
            {
                std::vector<uint32_t> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataTypeWBool::Double:
            {
                std::vector<double> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataTypeWBool::Float:
            {
                std::vector<float> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataTypeWBool::Char:
            {
                std::vector<int8_t> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataTypeWBool::UChar:
            {
                std::vector<uint8_t> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataTypeWBool::Bool:
            {
                std::vector<int8_t> col;
                col.reserve(rows);
                df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            default:
            {
                return cpp::failure(std::string("invalid datatype!"));
            }
        }
    }
    auto colmapPrime = boolToChar(colmap);
    DF out(std::move(df), std::move(colmapPrime));
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
                return cpp::failure(fmt::format("malformed line at {}", r + 1));
            }
            // fmt::print("emplacing {}\n", std::string(loc,loc1));
            switch(dtype)
            {
                case DataTypeWBool::Int:
                {
                    auto& vec = *((std::vector<int32_t>*)ptrs[idx]);
                    int32_t val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val);
                    break;
                }
                case DataTypeWBool::UInt:
                {
                    auto& vec = *((std::vector<uint32_t>*)ptrs[idx]);
                    uint32_t val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val);
                    break;
                }
                case DataTypeWBool::Double:
                {
                    auto& vec = *((std::vector<double>*)ptrs[idx]);
                    double val = strtod(loc, nullptr);
                    vec.emplace_back(val);
                    break;
                }
                case DataTypeWBool::Float:
                {
                    auto& vec = *((std::vector<float>*)ptrs[idx]);
                    float val = strtof(loc, nullptr);
                    vec.emplace_back(val);
                    break;
                }
                case DataTypeWBool::Char:
                {
                    auto& vec = *((std::vector<int8_t>*)ptrs[idx]);
                    int8_t val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val);
                    break;
                }
                case DataTypeWBool::UChar:
                {
                    auto& vec = *((std::vector<uint8_t>*)ptrs[idx]);
                    uint8_t val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val);
                    break;
                }
                case DataTypeWBool::Bool:
                {
                    auto& vec = *((std::vector<int8_t>*)ptrs[idx]);
                    int8_t val;
                    std::from_chars(loc, loc1, val);
                    vec.emplace_back(val > 0 ? 1 : 0);
                    break;
                }
                default:
                {
                    return cpp::failure(std::string("invalid datatype!"));
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
    return std::move(out);
}

}