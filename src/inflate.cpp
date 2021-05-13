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
#include "print_frame.h"

constexpr int BS = 1048576; // 1 GB

std::vector<std::string> split(const std::string& s)
{
    std::vector<std::string> out;
    std::size_t pos = 0;
    std::size_t pos1 = std::string::npos;
    do {
        pos1 = s.find(',', pos);
        if (pos1 != std::string::npos)
        {
            out.emplace_back(s.substr(pos, pos1-pos));
            pos = pos1+1;
        }
    } while (pos1 != std::string::npos && pos != 0);
    out.emplace_back(s.substr(pos));
    return out;
}

void inflate(const char* fn, std::map<std::string, std::string> converts)
{
    FILE*   f;
    BZFILE* b;
    int     nBuf;
    char    buf[BS];
    int     bzerror;
    int     nWritten;

    f = fopen(fn, "r");
    if (!f) {
        perror(fn);
        exit(1);
    }
    b = BZ2_bzReadOpen ( &bzerror, f, 0, 0, NULL, 0);
    if ( bzerror != BZ_OK ) {
        BZ2_bzReadClose ( &bzerror, b );
        Util::die("bzerror: {}\n", bzerror);
    }

    bzerror = BZ_OK;
    while ( bzerror == BZ_OK ) {
        nBuf = BZ2_bzRead ( &bzerror, b, buf, BS);
        if (bzerror == BZ_OK) { // returns BZ_STREAM_END when file ends
            Util::die("buff too small\n");
        }
    }
    if (bzerror != BZ_STREAM_END) {
        BZ2_bzReadClose( &bzerror, b );
        Util::die("bzerror: {}\n", bzerror);
    } else {
        BZ2_bzReadClose( &bzerror, b );
    }
    fclose(f);
    auto fstlne = std::find(buf, buf+nBuf, '\n');
    auto sndlne = std::find(fstlne+1, buf+nBuf, '\n');
    auto fstln = std::string(buf, fstlne);
    auto sndln = std::string(fstlne+1, sndlne);
    const auto colnames = split(fstln);
    const auto fields = split(sndln);
    DataFrame df;
    ColMap colmap = {
        {0,{"LIMIT_BAL",DataType::String}},
        {1,{"SEX",DataType::Int}},
        {2,{"EDUCATION",DataType::String}},
        {3,{"MARRIAGE",DataType::String}},
        {4,{"AGE",DataType::String}},
        {5,{"PAY_0",DataType::String}},
        {6,{"PAY_2",DataType::String}},
        {7,{"PAY_3",DataType::String}},
        {8,{"PAY_4",DataType::String}},
        {9,{"PAY_5",DataType::String}},
        {10,{"PAY_6",DataType::String}},
        {11,{"BILL_AMT1",DataType::String}},
        {12,{"BILL_AMT2",DataType::String}},
        {13,{"BILL_AMT3",DataType::String}},
        {14,{"BILL_AMT4",DataType::String}},
        {15,{"BILL_AMT5",DataType::String}},
        {16,{"BILL_AMT6",DataType::String}},
        {17,{"PAY_AMT1",DataType::String}},
        {18,{"PAY_AMT2",DataType::String}},
        {19,{"PAY_AMT3",DataType::String}},
        {20,{"PAY_AMT4",DataType::String}},
        {21,{"PAY_AMT5",DataType::String}},
        {22,{"PAY_AMT6",DataType::String}},
        {23,{"default.payment.next.month",DataType::String}}
    };

    const auto rows = std::count(buf, buf+nBuf, '\n') - 1;
    std::vector<IdxT> idx(rows);
    std::iota(idx.begin(), idx.end(), 0);
    df.load_index(std::move(idx));
    for (const auto& colname : colnames)
    {
        if (converts.count(colname) > 0)
        {
            std::vector<int> col;
            col.reserve(rows);
            df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
        }
        else
        {
            std::vector<std::string> col;
            col.reserve(rows);
            df.load_column(colname.c_str(), std::move(col), hmdf::nan_policy::dont_pad_with_nans);
        }
    }
    std::vector<const void*> ptrs;
    df::load_addresses(df, colmap, ptrs);
    auto loc = fstlne+1;
    const auto chars = {',', '\n'};
    for (long r = 0; r < rows; r++)
    {
        for (auto& [idx, pair] : colmap)
        {
            const auto& colname = pair.first;
            const auto dtype = pair.second;
            auto loc1 = std::find_first_of(loc, buf+nBuf, chars.begin(), chars.end());
            if (*loc1 == '\n' && idx != colnames.size() - 1)
            {
                Util::die("malformed line at {}\n", r + 1);
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
                case DataType::String:
                {
                    auto& vec = *((std::vector<std::string>*)ptrs[idx]);
                    vec.emplace_back(std::string(loc,loc1));
                    break;
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
    DataFrame df2 =
        df.get_data_by_idx<std::string, int>(hmdf::Index2D<IdxT> {0, 10});
    auto& col = df2.get_column<std::string>((size_t)0);
    fmt::print("{}\n", col);
    df::print(df2, colmap);
    df2.write<std::ostream, std::string, int>(std::cout, hmdf::io_format::csv2); 
    fmt::print("df shape: ({}, {})\n", df.shape().first, df.shape().second);
}