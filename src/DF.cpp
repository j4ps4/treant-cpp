#include <set>
#include <stdexcept>
#include <stdint.h>
#include <fmt/core.h>

#include "DF.h"

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

void DFR::assign_value(size_t cidx, double inval)
{
    auto dtype = colmap_->at(cidx).second;
    switch(dtype)
    {
        case DataType::Int:
        {
            row_.get_column<int32_t>(cidx)[0] = static_cast<int32_t>(inval);
            return;
        }
        case DataType::UInt:
        {
            row_.get_column<uint32_t>(cidx)[0] = static_cast<uint32_t>(inval);
            return;
        }
        case DataType::Double:
        {
            row_.get_column<double>(cidx)[0] = inval;
            return;
        }
        case DataType::Float:
        {
            row_.get_column<float>(cidx)[0] = static_cast<float>(inval);
            return;
        }
        case DataType::Char:
        {
            row_.get_column<int8_t>(cidx)[0] = static_cast<int8_t>(inval);
            return;
        }
        case DataType::UChar:
        {
            row_.get_column<uint8_t>(cidx)[0] = static_cast<uint8_t>(inval);
            return;
        }
    }
    throw std::runtime_error("invalid dtype???");
}


void DF::add_column(const char* colname, DataType dtype)
{
    auto newidx = colmap_.size();
    colmap_[newidx] = {std::string(colname), dtype};
    computeColMagic();
    switch(dtype)
    {
       case DataType::Int:
       {
           df_.load_column<int32_t>(colname, {}, hmdf::nan_policy::dont_pad_with_nans);
           return;
       } 
       case DataType::UInt:
       {
           df_.load_column<uint32_t>(colname, {}, hmdf::nan_policy::dont_pad_with_nans);
           return;
       }
       case DataType::Double:
       {
           df_.load_column<double>(colname, {}, hmdf::nan_policy::dont_pad_with_nans);
           return;
       } 
       case DataType::Float:
       {
           df_.load_column<float>(colname, {}, hmdf::nan_policy::dont_pad_with_nans);
           return;
       } 
       case DataType::Char:
       {
           df_.load_column<int8_t>(colname, {}, hmdf::nan_policy::dont_pad_with_nans);
           return;
       } 
       case DataType::UChar:
       {
           df_.load_column<uint8_t>(colname, {}, hmdf::nan_policy::dont_pad_with_nans);
           return;
       } 
    }
}

void DF::initializeFromColMap()
{
    for (auto& [idx, pair] : colmap_)
    {
        auto& colname = pair.first;
        auto dtype = pair.second;
        switch(dtype)
        {
            case DataType::Int:
            {
                df_.load_column<int32_t>(colname.c_str(),{}, hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::UInt:
            {
                df_.load_column<uint32_t>(colname.c_str(),{}, hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::Double:
            {
                df_.load_column<double>(colname.c_str(),{}, hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::Float:
            {
                df_.load_column<float>(colname.c_str(),{}, hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::Char:
            {
                df_.load_column<int8_t>(colname.c_str(),{}, hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
            case DataType::UChar:
            {
                df_.load_column<uint8_t>(colname.c_str(),{}, hmdf::nan_policy::dont_pad_with_nans);
                break;
            }
        }
    }
    //df_.load_index({});
}

void DF::append_row(const DFRView& view)
{
    // initialize this DF if empty
    if (colmap_.empty())
    {
        colmap_ = view.get_colmap();
        computeColMagic();
        initializeFromColMap();
    }
    for (auto& [idx, pair] : view.get_colmap())
    {
        auto& colname = pair.first;
        auto dtype = pair.second;
        switch(dtype)
        {
            case DataType::Int:
            {
                df_.get_column<int32_t>(idx).push_back(view.get<int32_t>(idx));
                break;
            }
            case DataType::UInt:
            {
                df_.get_column<uint32_t>(idx).push_back(view.get<uint32_t>(idx));
                break;
            }
            case DataType::Double:
            {
                df_.get_column<double>(idx).push_back(view.get<double>(idx));
                break;
            }
            case DataType::Float:
            {
                df_.get_column<float>(idx).push_back(view.get<float>(idx));
                break;
            }
            case DataType::Char:
            {
                df_.get_column<int8_t>(idx).push_back(view.get<int8_t>(idx));
                break;
            }
            case DataType::UChar:
            {
                df_.get_column<uint8_t>(idx).push_back(view.get<uint8_t>(idx));
                break;
            }
        }
    }
    if (df_.get_index().empty())
        df_.append_index(0);
    else
        df_.append_index(df_.get_index().back()+1);
}

void DF::append_row(const DFR& row)
{
    // initialize this DF if empty
    if (colmap_.empty())
    {
        colmap_ = row.get_colmap();
        computeColMagic();
        initializeFromColMap();
    }
    for (auto& [idx, pair] : row.get_colmap())
    {
        auto& colname = pair.first;
        auto dtype = pair.second;
        switch(dtype)
        {
            case DataType::Int:
            {
                df_.get_column<int32_t>(idx).push_back(row.get<int32_t>(idx));
                break;
            }
            case DataType::UInt:
            {
                df_.get_column<uint32_t>(idx).push_back(row.get<uint32_t>(idx));
                break;
            }
            case DataType::Double:
            {
                df_.get_column<double>(idx).push_back(row.get<double>(idx));
                break;
            }
            case DataType::Float:
            {
                df_.get_column<float>(idx).push_back(row.get<float>(idx));
                break;
            }
            case DataType::Char:
            {
                df_.get_column<int8_t>(idx).push_back(row.get<int8_t>(idx));
                break;
            }
            case DataType::UChar:
            {
                df_.get_column<uint8_t>(idx).push_back(row.get<uint8_t>(idx));
                break;
            }
        }
    }
    if (df_.get_index().empty())
        df_.append_index(0);
    else
        df_.append_index(df_.get_index().back()+1);
}

double DFR::get_as_f64(size_t cidx) const
{
    auto dtype = colmap_->at(cidx).second;
    switch(dtype)
    {
        case DataType::Int:
        {
            auto val = row_.get_column<int32_t>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::UInt:
        {
            auto val = row_.get_column<uint32_t>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::Double:
        {
            return row_.get_column<double>(cidx)[0];
        }
        case DataType::Float:
        {
            auto val = row_.get_column<float>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::Char:
        {
            auto val = row_.get_column<int8_t>(cidx)[0];
            return static_cast<double>(val);
        }
        case DataType::UChar:
        {
            auto val = row_.get_column<uint8_t>(cidx)[0];
            return static_cast<double>(val);
        }
    }
    throw std::runtime_error("invalid dtype???");
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

bool DFRView::operator==(const DFR& rhs) const
{
    if (get_colmap().size() != rhs.get_colmap().size())
        return false;
    for (const auto& [idx, pair] : get_colmap())
    {
        auto& colname = pair.first;
        auto dtype = pair.second;
        const auto& other = rhs.get_colmap().at(idx);
        if (other.first != colname || other.second != dtype)
            return false;
        switch(dtype)
        {
            case DataType::Int:
            {
                if (get<int32_t>(idx) != rhs.get<int32_t>(idx))
                    return false;
                break;
            }
            case DataType::UInt:
            {
                if (get<uint32_t>(idx) != rhs.get<uint32_t>(idx))
                    return false;
                break;
            }
            case DataType::Double:
            {
                if (get<double>(idx) != rhs.get<double>(idx))
                    return false;
                break;
            }
            case DataType::Float:
            {
                if (get<float>(idx) != rhs.get<float>(idx))
                    return false;
                break;
            }
            case DataType::Char:
            {
                if (get<int8_t>(idx) != rhs.get<int8_t>(idx))
                    return false;
                break;
            }
            case DataType::UChar:
            {
                if (get<uint8_t>(idx) != rhs.get<uint8_t>(idx))
                    return false;
                break;
            }
        }
    }
    return true;
}

bool DFRView::operator==(const DFRView& rhs) const
{
    if (this->parent_ == rhs.parent_ && this->idx_ == rhs.idx_)
        return true;
    if (get_colmap().size() != rhs.get_colmap().size())
        return false;
    for (const auto& [idx, pair] : get_colmap())
    {
        auto& colname = pair.first;
        auto dtype = pair.second;
        const auto& other = rhs.get_colmap().at(idx);
        if (other.first != colname || other.second != dtype)
            return false;
        switch(dtype)
        {
            case DataType::Int:
            {
                if (get<int32_t>(idx) != rhs.get<int32_t>(idx))
                    return false;
                break;
            }
            case DataType::UInt:
            {
                if (get<uint32_t>(idx) != rhs.get<uint32_t>(idx))
                    return false;
                break;
            }
            case DataType::Double:
            {
                if (get<double>(idx) != rhs.get<double>(idx))
                    return false;
                break;
            }
            case DataType::Float:
            {
                if (get<float>(idx) != rhs.get<float>(idx))
                    return false;
                break;
            }
            case DataType::Char:
            {
                if (get<int8_t>(idx) != rhs.get<int8_t>(idx))
                    return false;
                break;
            }
            case DataType::UChar:
            {
                if (get<uint8_t>(idx) != rhs.get<uint8_t>(idx))
                    return false;
                break;
            }
        }
    }
    return true;
}

bool DFRView::equal_disregarding(const DFR& rhs, const char* colname) const
{
    for (const auto& [idx, pair] : get_colmap())
    {
        auto& thiscolname = pair.first;
        if (thiscolname == colname)
            continue;
        auto dtype = pair.second;
        const auto& other = rhs.get_colmap().at(idx);
        if (other.first != thiscolname || other.second != dtype)
            return false;
        switch(dtype)
        {
            case DataType::Int:
            {
                if (get<int32_t>(idx) != rhs.get<int32_t>(idx))
                    return false;
                break;
            }
            case DataType::UInt:
            {
                if (get<uint32_t>(idx) != rhs.get<uint32_t>(idx))
                    return false;
                break;
            }
            case DataType::Double:
            {
                if (get<double>(idx) != rhs.get<double>(idx))
                    return false;
                break;
            }
            case DataType::Float:
            {
                if (get<float>(idx) != rhs.get<float>(idx))
                    return false;
                break;
            }
            case DataType::Char:
            {
                if (get<int8_t>(idx) != rhs.get<int8_t>(idx))
                    return false;
                break;
            }
            case DataType::UChar:
            {
                if (get<uint8_t>(idx) != rhs.get<uint8_t>(idx))
                    return false;
                break;
            }
        }
    }
    return true;
}

bool DFR::operator==(const DFRView& rhs) const
{
    return rhs.operator==(*this);
}

#include "DF.tpp"