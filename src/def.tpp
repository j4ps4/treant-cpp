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


DF DF::get_data_by_idx(hmdf::Index2D<IdxT> idx)
{
    $TPC\left(auto df = df_.get_data_by_idx\,(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);\right)
}

void DF::computeColMagic()
{
    auto dtypes = get_column_types(colmap_);
    nDtypes_ = dtypes.size();
    $COL_TP
}