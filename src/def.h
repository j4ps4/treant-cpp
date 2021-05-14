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

struct DF : public DataFrame
{
    DF(DataFrame&& df, ColMap&& colmap) :
        DataFrame(df), colmap_(colmap) {}
    DF(DataFrame&& df) : DataFrame(df), colmap_() {}
    ColMap colmap_;
};