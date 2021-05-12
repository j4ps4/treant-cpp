#pragma once

#include <fmt/core.h>

namespace Util {

template<typename... Args>
[[noreturn]] void die(const char* msg, Args... args)
{
    fmt::print(stderr, msg, args...);
    exit(1);
}

}