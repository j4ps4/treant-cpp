#include <set>
#include <stdexcept>

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

DF DF::get_data_by_idx(hmdf::Index2D<IdxT> idx) const
{
    $TPC\left(auto df = df_.get_data_by_idx\,(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);\right)
    Util::die("invalid magic ({},{})", nDtypes_, colMagic_);
}

DFView DF::get_view_by_idx(IdxT idx) const
{
    if (idx > shape().first - 1)
    {
        auto s = fmt::format("index {} too large for DataFrame of size {}", idx, shape().first);
        throw std::out_of_range(s);
    }
    auto slice = hmdf::Index2D<IdxT>{idx,idx};
    $TPC\left(auto view = df_.get_view_by_idx\,(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);\right)
    Util::die("invalid magic ({},{})", nDtypes_, colMagic_);
}

void DF::computeColMagic()
{
    auto dtypes = get_column_types(colmap_);
    nDtypes_ = dtypes.size();
    $COL_TP
}