#include <set>
#include <assert.h>

#include "def.h"

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

bool contains(const std::set<DataType> dtypes, DataType dt)
{
    return dtypes.count(dt) > 0;
}

}


DF DF::get_data_by_idx(hmdf::Index2D<IdxT> idx)
{
    auto dtypes = get_column_types(colmap_);
    if (contains(dtypes, DataType::Double) &&
        contains(dtypes, DataType::UInt) &&
        contains(dtypes, DataType::Short) &&
        contains(dtypes, DataType::Bool)
       )
    {
        auto df = df_.get_data_by_idx<double,unsigned int,short,bool>(idx);
        return DF(std::move(df), colmap_);
    }
    assert("cant return data!!");
}