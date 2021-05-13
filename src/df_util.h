#pragma once

#include "def.h"

namespace df
{

void load_addresses(const DataFrame& df,
                    const ColMap& colmap,
                    std::vector<const void*>& ptrs);


void print(const DataFrame& df,
           const std::map<size_t, std::pair<std::string, DataType>>& colmap);

}