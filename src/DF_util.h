#pragma once

#include "DF.h"

namespace df
{

void load_addresses(const DF& df,
                    std::vector<const void*>& ptrs);


void print(const DF& df);
void print(const DFR& df);
void print(const DFRView& df);

}