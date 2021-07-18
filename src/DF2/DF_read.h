
#include <string>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string>
#include <vector>
#include <charconv>
#include <stdint.h>
#include <eigen3/Eigen/Dense>

#include <fmt/core.h>

#include "../result.hpp"
#include "DF_util.h"

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

void read_val(char* start, char* end, double& val)
{
    val = strtod(start, nullptr);
}
void read_classid(char* start, char* end, size_t& val)
{
    std::from_chars(start, end, val);
    val = std::max(0UL, val);
}

static const auto chars = {',', '\n'};

template<size_t N>
void read_line(DF<N>& data, char* buf, int bufS, size_t row_id, char*& bufOut)
{
    char* bufE;
    Row<N> row = Eigen::ArrayXXd::Zero(1,N);
    for (size_t col = 0; col < N; col++)
    {
        bufE = std::find(buf, buf+bufS, ',');
        double val;
        read_val(buf, bufE, val);
        row(col) = val;
        buf = bufE+1;
    }
    data.row(row_id) = row;
    bufOut = bufE+1;
}

template<size_t N>
void read_encode_line(DF<N>& data, char* buf, int bufS, size_t row_id, char*& bufOut)
{
    char* bufE;
    Row<N> row = Eigen::ArrayXXd::Zero(1,N);
    for (size_t col = 0; col < N/2; col++)
    {
        bufE = std::find_first_of(buf, buf+bufS, chars.begin(), chars.end());
        size_t val;
        read_classid(buf, bufE, val);
        row(val) = 1.0;
        buf = bufE+1;
    }
    data.row(row_id) = row;
    bufOut = bufE+1;
}

}

namespace df {

template<size_t NX, size_t NY>
cpp::result<std::tuple<DF<NX>,DF<NY>>, std::string> read_bz2(const char* fn)
{
    int nBuf;
    size_t nl;
    std::unique_ptr<char[]> buf;

    auto res = read_buffer(fn, buf, nBuf, nl);
    if (res.has_error())
        return cpp::failure(res.error());

    auto fstlne = std::find(buf.get(), buf.get()+nBuf, '\n');
    //auto fstln = std::string(buf, fstlne);

    const auto rows = nl - 1;

    DF<NX> X_data = Eigen::ArrayXXd::Zero(rows,NX);
    DF<NY> Y_data = Eigen::ArrayXXd::Zero(rows,NY);
    auto loc = fstlne+1;
    for (size_t r = 0; r < rows; r++)
    {
        read_line<NX>(X_data, loc, nBuf, r, loc);
        read_encode_line<NY>(Y_data, loc, nBuf, r, loc);
    }
    return std::make_tuple(X_data, Y_data);
}

}
