#pragma once

#include <string>

#include "result.hpp"
#include "def.h"

namespace df
{

cpp::result<DF, std::string> read_bz2(const char* fn);

}