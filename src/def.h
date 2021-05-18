#pragma once
#include <DataFrame/DataFrame.h>

using IdxT = unsigned long;
using DataFrame = hmdf::StdDataFrame<IdxT>;

enum class DataType
{
    Int,
    UInt,
    Double,
    Float,
    Char,
    UChar,
    Bool
};

// Holds info about column names and types
using ColMap = std::map<size_t, std::pair<std::string, DataType>>;

// DataFrame augmented with bookkeeping info
struct DF
{
    explicit DF(DataFrame&& df, ColMap&& colmap) :
        df_(df), colmap_(colmap) {computeColMagic();}
    explicit DF(DataFrame&& df, const ColMap& colmap) :
        df_(df), colmap_(colmap) {computeColMagic();}

private:
    explicit DF(DataFrame&& df, const ColMap& colmap,
                unsigned int colMagic, size_t nDtypes) :
        df_(df), colmap_(colmap), colMagic_(colMagic), nDtypes_(nDtypes) {}

public:
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
private:
    void computeColMagic();
    unsigned int colMagic_;
    size_t nDtypes_;
};
