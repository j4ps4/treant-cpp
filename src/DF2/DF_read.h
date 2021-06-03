
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

cpp::result<void, std::string> read_buffer(const char* fn, std::unique_ptr<char[]>& buf, int& nBuf_out, size_t& nl)
{
    FILE*   f;
    BZFILE* b;
    size_t nLines = 0;
    int     nBuf = 0;
    int nBufTot = 0;
    int     bzerror;
    int     nWritten;

    constexpr int tmpSize = 524288;
    char tmpBuf[tmpSize];

    f = fopen(fn, "r");
    if (!f) {
        return cpp::failure(fmt::format("cannot open {}: {}", fn, strerror(errno)));
    }
    b = BZ2_bzReadOpen ( &bzerror, f, 0, 0, NULL, 0);
    if ( bzerror != BZ_OK ) {
        int bzerrorstore = bzerror;
        BZ2_bzReadClose ( &bzerror, b );
        return cpp::failure(fmt::format("bzerror: {}", bzerrorstore));
    }

    bzerror = BZ_OK;
    while ( bzerror == BZ_OK ) {
        nBuf = BZ2_bzRead ( &bzerror, b, tmpBuf, tmpSize);
        nBufTot += nBuf;
        nLines += std::count(tmpBuf, tmpBuf+nBuf, '\n');
        if (bzerror == BZ_STREAM_END) { // returns BZ_STREAM_END when file ends
            break;
        }
    }
    if (bzerror != BZ_STREAM_END) {
        int bzerrorstore = bzerror;
        BZ2_bzReadClose( &bzerror, b );
        return cpp::failure(fmt::format("bzerror: {}", bzerrorstore));
    } else {
        BZ2_bzReadClose( &bzerror, b );
    }

    auto arr = std::make_unique<char[]>(nBufTot);
    fseek(f, 0, SEEK_SET);
    b = BZ2_bzReadOpen ( &bzerror, f, 0, 0, NULL, 0);
    if ( bzerror != BZ_OK ) {
        int bzerrorstore = bzerror;
        BZ2_bzReadClose ( &bzerror, b );
        return cpp::failure(fmt::format("bzerror: {}", bzerrorstore));
    }

    bzerror = BZ_OK;
    nBuf = BZ2_bzRead ( &bzerror, b, arr.get(), nBufTot);
    if (bzerror == BZ_OK)
    {
        BZ2_bzReadClose ( &bzerror, b );
        return cpp::failure(std::string("couldn't read whole file into buffer!!!"));
    }
    if (bzerror != BZ_STREAM_END) {
        int bzerrorstore = bzerror;
        BZ2_bzReadClose( &bzerror, b );
        return cpp::failure(fmt::format("bzerror: {}", bzerrorstore));
    } else {
        BZ2_bzReadClose( &bzerror, b );
    }

    fclose(f);
    nBuf_out = nBufTot;
    nl = nLines;
    buf = std::move(arr);
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
    val = temp > 0 ? true : false;
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
    int nBuf;
    size_t nl;
    std::unique_ptr<char[]> buf;

    auto res = read_buffer(fn, buf, nBuf, nl);
    if (res.has_error())
        return cpp::failure(res.error());

    auto fstlne = std::find(buf.get(), buf.get()+nBuf, '\n');
    //auto fstln = std::string(buf, fstlne);

    const auto rows = nl - 1;

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
