
#include <string>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string>
#include <vector>
#include <charconv>
#include <stdint.h>

#include <fmt/core.h>

#include "../result.hpp"
#include "DF.h"

constexpr int BS = 1048576; // 1 GB

// template<size_t N>
// void parse_col_names(const std::string& line, std::array<FixedStr,N> colnames)
// {
//     size_t idx = 0;
//     std::size_t pos = 0;
//     std::size_t pos1 = std::string::npos;
//     do {
//         pos1 = line.find(',', pos);
//         if (pos1 != std::string::npos)
//         {
//             auto frag = line.substr(pos, pos1-pos);
//             std::strncpy(colnames[idx].data(), frag.c_str(), 64);
//             colnames[idx][63] = '\0';
//             pos = pos1+1;
//             idx++;
//         }
//     } while (pos1 != std::string::npos && pos != 0);
//     auto frag = line.substr(pos);
//     std::strncpy(colnames[idx].data(), frag.c_str(), 64);
//     colnames[idx][63] = '\0';
// }

namespace {

cpp::result<void, std::string> read_buffer(const char* fn, char* buf, int& nBuf_out)
{
    FILE*   f;
    BZFILE* b;
    int     nBuf;
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
    nBuf_out = nBuf;
    return {};
}

template<typename T>
void read(char* start, char* end, T& val)
{
    std::from_chars(start, end, val);
}

template<>
void read<double>(char* start, char* end, double& val)
{
    val = strtod(start, nullptr);
}
template<>
void read<float>(char* start, char* end, float& val)
{
    val = strtof(start, nullptr);
}
template<>
void read<bool>(char* start, char* end, bool& val)
{
    int8_t temp;
    std::from_chars(start, end, temp);
    val = temp > 0 ? 1 : 0;
}

static const auto chars = {',', '\n'};

template<typename T>
std::tuple<T> read_line(char* buf, int bufS, size_t col_idx, size_t NC, char*& bufOut)
{
    if (col_idx >= NC)
        throw std::runtime_error("malformed line");
    auto bufE = std::find_first_of(buf, buf+bufS, chars.begin(), chars.end());
    T val;
    read(buf, bufE, val);
    bufOut = bufE+1;
    return std::make_tuple(val);
}

template<typename T1, typename T2, typename... Ts>
std::tuple<T1,T2,Ts...> read_line(char* buf, int bufS, size_t col_idx, size_t NC, char*& bufOut)
{
    if (col_idx >= NC)
        throw std::runtime_error("malformed line");
    auto bufE = std::find_first_of(buf, buf+bufS, chars.begin(), chars.end());
    T1 val;
    read(buf, bufE, val);
    buf = bufE+1;
    return std::tuple_cat(std::make_tuple(val), read_line<T2,Ts...>(buf, bufS, col_idx+1, NC, bufOut));
}

}

namespace df {

template<typename... Ts>
cpp::result<DF<Ts...>, std::string> read_bz2(const char* fn)
{
    constexpr size_t NC = sizeof...(Ts);
    char    buf[BS];
    int nBuf;

    auto res = read_buffer(fn, buf, nBuf);
    if (res.has_error())
        return cpp::failure(res.error());

    auto fstlne = std::find(buf, buf+nBuf, '\n');
    //auto fstln = std::string(buf, fstlne);

    const auto rows = std::count(buf, buf+nBuf, '\n') - 1;

    std::vector<std::tuple<Ts...>> data;
    data.reserve(rows);
    auto loc = fstlne+1;
    for (long r = 0; r < rows; r++)
    {
        auto tuple = read_line<Ts...>(loc, nBuf, 0, NC, loc);
        data.push_back(tuple);
    }
    return DF<Ts...>(std::move(data));
}

}
