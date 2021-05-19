#pragma once
#include <DataFrame/DataFrame.h>

using IdxT = unsigned long;
using DataFrame = hmdf::StdDataFrame<IdxT>;
using DataFrameView = hmdf::DataFrameView<IdxT>;

enum class DataType
{
    Int,
    UInt,
    Double,
    Float,
    Char,
    UChar,
};
enum class DataTypeWBool
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
using ColMapWBool = std::map<size_t, std::pair<std::string, DataTypeWBool>>;

ColMap boolToChar(const ColMapWBool& colmap);

// View into single row
struct DFView
{
    explicit DFView(DataFrameView&& view, const ColMap* colmap,
                    unsigned int colMagic, size_t nDtypes) :
        view_(view), colmap_(colmap), colMagic_(colMagic), nDtypes_(nDtypes) {}

    template<typename T>
    const T& get(size_t cidx) const
    {
        return view_.template get_column<T>(cidx)[0];
    }
private:
    DataFrameView view_;
    const ColMap* colmap_;
    unsigned int colMagic_;
    size_t nDtypes_;
};

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
    DF get_data_by_idx(hmdf::Index2D<IdxT>) const;

    DFView get_view_by_idx(IdxT idx) const;

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

    const ColMap& get_colmap() const
    {
        return colmap_;
    }

private:
    DataFrame df_;
    ColMap colmap_;
    void computeColMagic();
    unsigned int colMagic_;
    size_t nDtypes_;
};
