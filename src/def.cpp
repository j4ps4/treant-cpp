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
    switch(this->nDtypes_){
  case 1:
  {
    switch(this->colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int32_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<uint32_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<double>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 2:
  {
    switch(this->colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int32_t,double>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int32_t,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int32_t,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int32_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<uint32_t,double>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto df = df_.get_data_by_idx<uint32_t,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto df = df_.get_data_by_idx<uint32_t,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto df = df_.get_data_by_idx<uint32_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto df = df_.get_data_by_idx<double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto df = df_.get_data_by_idx<double,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto df = df_.get_data_by_idx<double,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto df = df_.get_data_by_idx<float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto df = df_.get_data_by_idx<float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto df = df_.get_data_by_idx<int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 3:
  {
    switch(this->colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int32_t,double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<int32_t,double,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto df = df_.get_data_by_idx<int32_t,double,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto df = df_.get_data_by_idx<int32_t,float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto df = df_.get_data_by_idx<int32_t,float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto df = df_.get_data_by_idx<int32_t,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto df = df_.get_data_by_idx<uint32_t,double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto df = df_.get_data_by_idx<uint32_t,double,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto df = df_.get_data_by_idx<uint32_t,double,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto df = df_.get_data_by_idx<uint32_t,float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto df = df_.get_data_by_idx<uint32_t,float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 15:{
    auto df = df_.get_data_by_idx<uint32_t,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 16:{
    auto df = df_.get_data_by_idx<double,float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 17:{
    auto df = df_.get_data_by_idx<double,float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 18:{
    auto df = df_.get_data_by_idx<double,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 19:{
    auto df = df_.get_data_by_idx<float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 4:
  {
    switch(this->colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto df = df_.get_data_by_idx<int32_t,double,float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto df = df_.get_data_by_idx<int32_t,double,float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto df = df_.get_data_by_idx<int32_t,double,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto df = df_.get_data_by_idx<int32_t,float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto df = df_.get_data_by_idx<uint32_t,double,float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto df = df_.get_data_by_idx<uint32_t,double,float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto df = df_.get_data_by_idx<uint32_t,double,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto df = df_.get_data_by_idx<uint32_t,float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto df = df_.get_data_by_idx<double,float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 5:
  {
    switch(this->colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double,float,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int32_t,double,float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<uint32_t,double,float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 6:
  {
    switch(this->colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t,uint8_t>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
}
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
    switch(this->nDtypes_){
  case 1:
  {
    switch(this->colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int32_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 1:{
    auto view = df_.get_view_by_idx<uint32_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 2:{
    auto view = df_.get_view_by_idx<double>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 3:{
    auto view = df_.get_view_by_idx<float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 5:{
    auto view = df_.get_view_by_idx<uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    }
  }
  case 2:
  {
    switch(this->colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int32_t,double>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int32_t,float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int32_t,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int32_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 5:{
    auto view = df_.get_view_by_idx<uint32_t,double>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 6:{
    auto view = df_.get_view_by_idx<uint32_t,float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 7:{
    auto view = df_.get_view_by_idx<uint32_t,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 8:{
    auto view = df_.get_view_by_idx<uint32_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 9:{
    auto view = df_.get_view_by_idx<double,float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 10:{
    auto view = df_.get_view_by_idx<double,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 11:{
    auto view = df_.get_view_by_idx<double,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 12:{
    auto view = df_.get_view_by_idx<float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 13:{
    auto view = df_.get_view_by_idx<float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 14:{
    auto view = df_.get_view_by_idx<int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    }
  }
  case 3:
  {
    switch(this->colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int32_t,double,float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 5:{
    auto view = df_.get_view_by_idx<int32_t,double,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 6:{
    auto view = df_.get_view_by_idx<int32_t,double,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 7:{
    auto view = df_.get_view_by_idx<int32_t,float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 8:{
    auto view = df_.get_view_by_idx<int32_t,float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 9:{
    auto view = df_.get_view_by_idx<int32_t,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 10:{
    auto view = df_.get_view_by_idx<uint32_t,double,float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 11:{
    auto view = df_.get_view_by_idx<uint32_t,double,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 12:{
    auto view = df_.get_view_by_idx<uint32_t,double,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 13:{
    auto view = df_.get_view_by_idx<uint32_t,float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 14:{
    auto view = df_.get_view_by_idx<uint32_t,float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 15:{
    auto view = df_.get_view_by_idx<uint32_t,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 16:{
    auto view = df_.get_view_by_idx<double,float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 17:{
    auto view = df_.get_view_by_idx<double,float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 18:{
    auto view = df_.get_view_by_idx<double,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 19:{
    auto view = df_.get_view_by_idx<float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    }
  }
  case 4:
  {
    switch(this->colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double,float>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 5:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 6:{
    auto view = df_.get_view_by_idx<int32_t,double,float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 7:{
    auto view = df_.get_view_by_idx<int32_t,double,float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 8:{
    auto view = df_.get_view_by_idx<int32_t,double,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 9:{
    auto view = df_.get_view_by_idx<int32_t,float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 10:{
    auto view = df_.get_view_by_idx<uint32_t,double,float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 11:{
    auto view = df_.get_view_by_idx<uint32_t,double,float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 12:{
    auto view = df_.get_view_by_idx<uint32_t,double,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 13:{
    auto view = df_.get_view_by_idx<uint32_t,float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 14:{
    auto view = df_.get_view_by_idx<double,float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    }
  }
  case 5:
  {
    switch(this->colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double,float,int8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double,float,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int32_t,double,float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    case 5:{
    auto view = df_.get_view_by_idx<uint32_t,double,float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    }
  }
  case 6:
  {
    switch(this->colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int32_t,uint32_t,double,float,int8_t,uint8_t>(slice);
    return DFRView(std::move(view), idx, this);
    }
    }
  }
}
    auto s = fmt::format("invalid magic ({},{})", nDtypes_, colMagic_);
    throw std::runtime_error(s);
}

DFR DFRView::copy() const
{
    auto idx = hmdf::Index2D<IdxT>{idx_,idx_};
    switch(parent_->nDtypes_){
  case 1:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<uint32_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<double>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 2:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 6:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 7:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 8:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 9:{
    auto df = parent_->df_.get_data_by_idx<double,float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 10:{
    auto df = parent_->df_.get_data_by_idx<double,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 11:{
    auto df = parent_->df_.get_data_by_idx<double,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 12:{
    auto df = parent_->df_.get_data_by_idx<float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 13:{
    auto df = parent_->df_.get_data_by_idx<float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 14:{
    auto df = parent_->df_.get_data_by_idx<int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 3:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 6:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 7:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 8:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 9:{
    auto df = parent_->df_.get_data_by_idx<int32_t,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 10:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 11:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 12:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 13:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 14:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 15:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 16:{
    auto df = parent_->df_.get_data_by_idx<double,float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 17:{
    auto df = parent_->df_.get_data_by_idx<double,float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 18:{
    auto df = parent_->df_.get_data_by_idx<double,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 19:{
    auto df = parent_->df_.get_data_by_idx<float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 4:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 6:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 7:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 8:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 9:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 10:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 11:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 12:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 13:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 14:{
    auto df = parent_->df_.get_data_by_idx<double,float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 5:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 6:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t,uint8_t>(idx);
    return DFR(df, &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
}
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
    switch(nDtypes_){
  case 1:
  {
    if (contains(dtypes,DataType::Int)){colMagic_ = 0; return;}
    if (contains(dtypes,DataType::UInt)){colMagic_ = 1; return;}
    if (contains(dtypes,DataType::Double)){colMagic_ = 2; return;}
    if (contains(dtypes,DataType::Float)){colMagic_ = 3; return;}
    if (contains(dtypes,DataType::Char)){colMagic_ = 4; return;}
    if (contains(dtypes,DataType::UChar)){colMagic_ = 5; return;}
  }
  case 2:
  {
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 0; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double)){colMagic_ = 1; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Float)){colMagic_ = 2; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Char)){colMagic_ = 3; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 4; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double)){colMagic_ = 5; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float)){colMagic_ = 6; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Char)){colMagic_ = 7; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::UChar)){colMagic_ = 8; return;}
    if (contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 9; return;}
    if (contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char)){colMagic_ = 10; return;}
    if (contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 11; return;}
    if (contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 12; return;}
    if (contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 13; return;}
    if (contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 14; return;}
  }
  case 3:
  {
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double)){colMagic_ = 0; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float)){colMagic_ = 1; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Char)){colMagic_ = 2; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::UChar)){colMagic_ = 3; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 4; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char)){colMagic_ = 5; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 6; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 7; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 8; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 9; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 10; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char)){colMagic_ = 11; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 12; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 13; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 14; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 15; return;}
    if (contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 16; return;}
    if (contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 17; return;}
    if (contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 18; return;}
    if (contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 19; return;}
  }
  case 4:
  {
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 0; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char)){colMagic_ = 1; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 2; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 3; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 4; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 5; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 6; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 7; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 8; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 9; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 10; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 11; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 12; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 13; return;}
    if (contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 14; return;}
  }
  case 5:
  {
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char)){colMagic_ = 0; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 1; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 2; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 3; return;}
    if (contains(dtypes,DataType::Int) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 4; return;}
    if (contains(dtypes,DataType::UInt) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 5; return;}
  }
  case 6: {colMagic_ = 0; return;}
}
throw std::runtime_error("too many datatypes");
}