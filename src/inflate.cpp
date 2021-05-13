#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <charconv>

#include <fmt/core.h>
#include <DataFrame/DataFrame.h>

#include "util.h"

constexpr int BS = 1048576; // 1 GB
using IdxT = unsigned long;
using DataFrame = hmdf::StdDataFrame<IdxT>;

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
            df.load_column(colname.c_str(), std::move(col));
        }
        else
        {
            std::vector<std::string> col;
            col.reserve(rows);
            df.load_column(colname.c_str(), std::move(col));
        }
    }
    auto loc = fstlne+1;
    const auto chars = {',', '\n'};
    for (long r = 0; r < rows; r++)
    {
        for (std::size_t c = 0; c < colnames.size(); c++)
        {
            const auto& colname = colnames[c];
            auto loc1 = std::find_first_of(loc, buf+nBuf, chars.begin(), chars.end());
            if (*loc1 == '\n' && c != colnames.size() - 1)
            {
                Util::die("malformed line at {}\n", r + 1);
            }
            // fmt::print("emplacing {}\n", std::string(loc,loc1));
            if (converts.count(colname) > 0)
            {
                int val;
                std::from_chars(loc, loc1, val);
                df.get_column<int>(c).emplace_back(val);
            }
            else
            {
                df.get_column<std::string>(c).emplace_back(std::string(loc,loc1));
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
    fmt::print("{}\n", df.get_column<int>((std::size_t)1));
    fmt::print("df shape: ({}, {})\n", df.shape().first, df.shape().second);
}