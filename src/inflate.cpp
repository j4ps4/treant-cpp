#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string>
#include <map>
#include <vector>

#include "util.h"

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

    f = fopen (fn, "r" );
    if ( !f ) {
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
    auto colnames = split(fstln);
    auto fields = split(sndln);
    fmt::print("fst line: {}\n", colnames);
    fmt::print("snd line: {}\n", fields);
    auto rows = std::count(buf, buf+nBuf, '\n') - 1;
    fmt::print("rows: {}\n", rows);
}