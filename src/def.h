#pragma once
#include <DataFrame/DataFrame.h>

using IdxT = std::size_t;
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

struct DF;

// Single row
struct DFR
{
    explicit DFR(const DataFrame& row, const ColMap* colmap,
                 unsigned int colMagic, size_t nDtypes) :
        row_(row), colmap_(colmap), colMagic_(colMagic), nDtypes_(nDtypes) {}

    void modify_value(size_t cidx, double inval);

private:
    DataFrame row_;
    const ColMap* colmap_;
    unsigned int colMagic_;
    size_t nDtypes_;
};

// View into single row
struct DFRView
{
    explicit DFRView(DataFrameView&& view, IdxT idx, const DF* ptr) :
        view_(view), idx_(idx), parent_(ptr) {}

    template<typename T>
    const T& get(size_t cidx) const
    {
        return view_.template get_column<T>(cidx)[0];
    }

    double get_as_f64(size_t cidx) const;
    int32_t get_as_i32(size_t cidx) const;
    int8_t get_as_i8(size_t cidx) const;

    // Make a copy of row where this view points to
    DFR copy() const;

    const ColMap& get_colmap() const noexcept;

private:
    DataFrameView view_;
    IdxT idx_; // row idx in dataframe
    const DF* parent_; // pointer to dataframe
};


// DataFrame augmented with bookkeeping info
struct DF
{
    friend struct DFRView;
    explicit DF(DataFrame&& df, ColMap&& colmap) :
        df_(df), colmap_(colmap) {computeColMagic();}
    explicit DF(DataFrame&& df, const ColMap& colmap) :
        df_(df), colmap_(colmap) {computeColMagic();}
    DF(DF&& df) = default;

private:
    explicit DF(DataFrame&& df, const ColMap& colmap,
                unsigned int colMagic, size_t nDtypes) :
        df_(df), colmap_(colmap), colMagic_(colMagic), nDtypes_(nDtypes) {}

public:
    DF get_data_by_idx(hmdf::Index2D<IdxT>) const;

    DFRView get_row_view(IdxT idx) const;

    auto shape() const noexcept {return df_.shape();}

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

    const ColMap& get_colmap() const noexcept
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
