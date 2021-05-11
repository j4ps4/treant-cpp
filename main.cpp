#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>

constexpr int BS = 65536;

template<typename... Args>
[[noreturn]] void die(const char* msg, Args... args)
{
    fmt::print(stderr, msg, args...);
    exit(1);
}

int main(int argc, char** argv)
{
    FILE*   f;
    BZFILE* b;
    int     nBuf;
    char    buf[BS];
    int     bzerror;
    int     nWritten;

    if (argc < 2)
    {
        fmt::print(stderr, "usage\n");
        return 1;
    }

    f = fopen (argv[1], "r" );
    if ( !f ) {
        perror("fopen");
        return 1;
    }
    b = BZ2_bzReadOpen ( &bzerror, f, 0, 0, NULL, 0);
    if ( bzerror != BZ_OK ) {
        BZ2_bzReadClose ( &bzerror, b );
        die("bzerror: {}\n", bzerror);
    }

    bzerror = BZ_OK;
    while ( bzerror == BZ_OK ) {
        nBuf = BZ2_bzRead ( &bzerror, b, buf, BS);
    if ( bzerror == BZ_OK ) {
        fwrite(buf, sizeof(char), nBuf, stdout);
    }
    }
    if ( bzerror != BZ_STREAM_END ) {
        BZ2_bzReadClose ( &bzerror, b );
        die("bzerror: {}\n", bzerror);
    } else {
        BZ2_bzReadClose ( &bzerror, b );
    }
    fclose(f);
}