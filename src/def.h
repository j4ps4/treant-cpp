#pragma once
#include <DataFrame/DataFrame.h>

using IdxT = unsigned long;
using DataFrame = hmdf::StdDataFrame<IdxT>;

enum class DataType
{
    String,
    Int,
    Double
};

using ColMap = std::map<size_t, std::pair<std::string, DataType>>;