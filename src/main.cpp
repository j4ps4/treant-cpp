#include <fmt/core.h>
#include <bzlib.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"
#include "inflate.h"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        Util::die("usage\n");
    }

    auto fn = argv[1];
    inflate(fn, {{"SEX", "int"}});
}