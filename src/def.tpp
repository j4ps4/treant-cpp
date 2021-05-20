#include <set>
#include <stdexcept>
#include <stdint.h>

#include "def.h"
#include "util.h"

namespace
{

std::set<DataType> get_column_types(const ColMap& colmap)
{
    std::set<DataType> out;
    for (const auto& [idx, pair] : colmap)
    {
        out.insert(pair.second);
    }
    return out;
}

inline bool contains(const std::set<DataType>& dtypes, DataType dt)
{
    return dtypes.count(dt) > 0;
}

}

ColMap boolToChar(const ColMapWBool& colmap)
{
    ColMap out;
    for (const auto& [idx, pair] : colmap)
    {
        const auto& colname = pair.first;
        const auto dtype = pair.second;
        switch(dtype)
        {
            case DataTypeWBool::Bool:
            {
                out[idx] = std::make_pair(colname, DataType::Char);
                break;
            }
            default:
            {
                out[idx] = std::make_pair(colname, static_cast<DataType>(dtype));
            }
        }
    }
    return out;
}

void DFR::modify_value(size_t cidx, double inval)
{
    auto dtype = colmap_->at(cidx).second;
    switch(dtype)
    {
        case DataType::Int:
        {
            row_.get_column<int32_t>(cidx)[0] += inval;
            return;
        }
        case DataType::UInt:
        {
            row_.get_column<uint32_t>(cidx)[0] += inval;
            return;
        }
        case DataType::Double:
        {
            row_.get_column<double>(cidx)[0] += inval;
            return;
        }
        case DataType::Float:
        {
            row_.get_column<float>(cidx)[0] += inval;
            return;
        }
        case DataType::Char:
        {
            row_.get_column<int8_t>(cidx)[0] += inval;
            return;
        }
        case DataType::UChar:
        {
            row_.get_column<uint8_t>(cidx)[0] += inval;
            return;
        }
    }
    throw std::runtime_error("invalid dtype???");
}

DF DF::get_data_by_idx(hmdf::Index2D<IdxT> idx) const
{
    $TPC1\left(auto df = df_.get_data_by_idx\,(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);\right)
    auto s = fmt::format("invalid magic ({},{})", nDtypes_, colMagic_);
    throw std::runtime_error(s);
}

DFRView DF::get_row_view(IdxT idx) const
{
    if (idx > shape().first - 1)
    {
        auto s = fmt::format("index {} too large for DataFrame of size {}", idx, shape().first);
        throw std::out_of_range(s);
    }
    auto slice = hmdf::Index2D<IdxT>{idx,idx};
    $TPC1\left(auto view = df_.get_view_by_idx\,(slice);
    return DFRView(std::move(view), idx, this);\right)
    auto s = fmt::format("invalid magic ({},{})", nDtypes_, colMagic_);
    throw std::runtime_error(s);
}

DFR DFRView::copy() const
{
    auto idx = hmdf::Index2D<IdxT>{idx_,idx_};
    $TPC2\left(auto df = parent_->df_.get_data_by_idx\,(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);\right)
    auto s = fmt::format("invalid magic ({},{})", parent_->nDtypes_, parent_->colMagic_);
    throw std::runtime_error(s);
}

const ColMap& DFRView::get_colmap() const noexcept
{
    return parent_->colmap_;
}

double DFRView::get_as_f64(size_t cidx) const
{
    auto dtype = parent_->colmap_.at(cidx).second;
    switch(dtype)
    {
        case DataType::Int:
        {
            auto val = view_.get_column<int32_t>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::UInt:
        {
            auto val = view_.get_column<uint32_t>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::Double:
        {
            return view_.get_column<double>(cidx)[0];
        }
        case DataType::Float:
        {
            auto val = view_.get_column<float>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::Char:
        {
            auto val = view_.get_column<int8_t>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::UChar:
        {
            auto val = view_.get_column<uint8_t>(cidx)[0];
            return static_cast<double>(val);
        }
    }
    throw std::runtime_error("invalid dtype???");
}
int32_t DFRView::get_as_i32(size_t cidx) const
{
    auto dtype = parent_->colmap_.at(cidx).second;
    switch(dtype)
    {
        case DataType::Int:
        {
            return view_.get_column<int32_t>(cidx)[0];
        }
        case DataType::UInt:
        {
            auto val = view_.get_column<uint32_t>(cidx)[0];
            return static_cast<int32_t>(val);
        }
        case DataType::Double:
        {
            auto val = view_.get_column<double>(cidx)[0];
            return static_cast<int32_t>(val);
        }
        case DataType::Float:
        {
            auto val = view_.get_column<float>(cidx)[0];
            return static_cast<int32_t>(val);
        }
        case DataType::Char:
        {
            auto val = view_.get_column<int8_t>(cidx)[0];
            return static_cast<int32_t>(val);
        }
        case DataType::UChar:
        {
            auto val = view_.get_column<uint8_t>(cidx)[0];
            return static_cast<int32_t>(val);
        }
    }
    throw std::runtime_error("invalid dtype???");
}
int8_t DFRView::get_as_i8(size_t cidx) const
{
    auto dtype = parent_->colmap_.at(cidx).second;
    switch(dtype)
    {
        case DataType::Int:
        {
            auto val = view_.get_column<int32_t>(cidx)[0];
            return static_cast<int8_t>(val);
        }
        case DataType::UInt:
        {
            auto val = view_.get_column<uint32_t>(cidx)[0];
            return static_cast<int8_t>(val);
        }
        case DataType::Double:
        {
            auto val = view_.get_column<double>(cidx)[0];
            return static_cast<int8_t>(val);
        }
        case DataType::Float:
        {
            auto val = view_.get_column<float>(cidx)[0];
            return static_cast<int8_t>(val);
        }
        case DataType::Char:
        {
            return view_.get_column<int8_t>(cidx)[0];
        }
        case DataType::UChar:
        {
            auto val = view_.get_column<uint8_t>(cidx)[0];
            return static_cast<int8_t>(val);
        }
    }
    throw std::runtime_error("invalid dtype???");
}

void DF::computeColMagic()
{
    auto dtypes = get_column_types(colmap_);
    nDtypes_ = dtypes.size();
    $COL_TP
}