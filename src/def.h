#pragma once
#include <DataFrame/DataFrame.h>

using IdxT = unsigned long;
using DataFrame = hmdf::StdDataFrame<IdxT>;

enum class DataType
{
    String,
    Int,
    UInt,
    Double,
    Float,
    Short,
    UShort,
    Bool
};

// Holds info about column names and types
using ColMap = std::map<size_t, std::pair<std::string, DataType>>;

// DataFrame augmented with bookkeeping info
struct DF
{
    DF(DataFrame&& df, ColMap&& colmap) :
        df_(df), colmap_(colmap) {}
    DF(DataFrame&& df, const ColMap& colmap) :
        df_(df), colmap_(colmap) {}

    DF get_data_by_idx(hmdf::Index2D<IdxT>);

    auto shape() const {return df_.shape();}

    template<typename T>
    const std::vector<T>& get_column(const char* col) const
    {
        return df_.template get_column<T>(col);
    }

    template<typename T>
    std::vector<T>& get_column(const char* col)
    {
        return df_.template get_column<T>(col);
    }

    DataFrame df_;
    ColMap colmap_;
};