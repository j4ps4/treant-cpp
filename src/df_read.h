#pragma once

#include <string>
#include <variant>

#include "def.h"

namespace df
{

std::variant<DataFrame, std::string> read_bz2(const char* fn);

}