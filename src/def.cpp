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
    switch(nDtypes_){
  case 1:
  {
    switch(colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<unsigned int>(idx);
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
    auto df = df_.get_data_by_idx<signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 2:
  {
    switch(colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int,unsigned int>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int,double>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<unsigned int,double>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto df = df_.get_data_by_idx<unsigned int,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto df = df_.get_data_by_idx<unsigned int,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto df = df_.get_data_by_idx<unsigned int,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto df = df_.get_data_by_idx<double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto df = df_.get_data_by_idx<double,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto df = df_.get_data_by_idx<double,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto df = df_.get_data_by_idx<float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto df = df_.get_data_by_idx<float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto df = df_.get_data_by_idx<signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 3:
  {
    switch(colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int,unsigned int,double>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int,unsigned int,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int,unsigned int,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int,unsigned int,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int,double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<int,double,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto df = df_.get_data_by_idx<int,double,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto df = df_.get_data_by_idx<int,float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto df = df_.get_data_by_idx<int,float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto df = df_.get_data_by_idx<int,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto df = df_.get_data_by_idx<unsigned int,double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto df = df_.get_data_by_idx<unsigned int,double,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto df = df_.get_data_by_idx<unsigned int,double,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto df = df_.get_data_by_idx<unsigned int,float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto df = df_.get_data_by_idx<unsigned int,float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 15:{
    auto df = df_.get_data_by_idx<unsigned int,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 16:{
    auto df = df_.get_data_by_idx<double,float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 17:{
    auto df = df_.get_data_by_idx<double,float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 18:{
    auto df = df_.get_data_by_idx<double,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 19:{
    auto df = df_.get_data_by_idx<float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 4:
  {
    switch(colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int,unsigned int,double,float>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int,unsigned int,double,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int,unsigned int,double,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int,unsigned int,float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int,unsigned int,float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<int,unsigned int,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto df = df_.get_data_by_idx<int,double,float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto df = df_.get_data_by_idx<int,double,float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto df = df_.get_data_by_idx<int,double,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto df = df_.get_data_by_idx<int,float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto df = df_.get_data_by_idx<unsigned int,double,float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto df = df_.get_data_by_idx<unsigned int,double,float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto df = df_.get_data_by_idx<unsigned int,double,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto df = df_.get_data_by_idx<unsigned int,float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto df = df_.get_data_by_idx<double,float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 5:
  {
    switch(colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int,unsigned int,double,float,signed char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto df = df_.get_data_by_idx<int,unsigned int,double,float,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto df = df_.get_data_by_idx<int,unsigned int,double,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto df = df_.get_data_by_idx<int,unsigned int,float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto df = df_.get_data_by_idx<int,double,float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto df = df_.get_data_by_idx<unsigned int,double,float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 6:
  {
    switch(colMagic_){
    case 0:{
    auto df = df_.get_data_by_idx<int,unsigned int,double,float,signed char,unsigned char>(idx);
    return DF(std::move(df), colmap_, colMagic_, nDtypes_);
    }
    }
  }
}

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
    switch(nDtypes_){
  case 1:
  {
    switch(colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto view = df_.get_view_by_idx<unsigned int>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto view = df_.get_view_by_idx<double>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto view = df_.get_view_by_idx<float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto view = df_.get_view_by_idx<signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto view = df_.get_view_by_idx<unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 2:
  {
    switch(colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int,unsigned int>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int,double>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int,float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto view = df_.get_view_by_idx<unsigned int,double>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto view = df_.get_view_by_idx<unsigned int,float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto view = df_.get_view_by_idx<unsigned int,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto view = df_.get_view_by_idx<unsigned int,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto view = df_.get_view_by_idx<double,float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto view = df_.get_view_by_idx<double,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto view = df_.get_view_by_idx<double,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto view = df_.get_view_by_idx<float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto view = df_.get_view_by_idx<float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto view = df_.get_view_by_idx<signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 3:
  {
    switch(colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int,unsigned int,double>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int,unsigned int,float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int,unsigned int,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int,unsigned int,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int,double,float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto view = df_.get_view_by_idx<int,double,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto view = df_.get_view_by_idx<int,double,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto view = df_.get_view_by_idx<int,float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto view = df_.get_view_by_idx<int,float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto view = df_.get_view_by_idx<int,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto view = df_.get_view_by_idx<unsigned int,double,float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto view = df_.get_view_by_idx<unsigned int,double,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto view = df_.get_view_by_idx<unsigned int,double,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto view = df_.get_view_by_idx<unsigned int,float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto view = df_.get_view_by_idx<unsigned int,float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 15:{
    auto view = df_.get_view_by_idx<unsigned int,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 16:{
    auto view = df_.get_view_by_idx<double,float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 17:{
    auto view = df_.get_view_by_idx<double,float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 18:{
    auto view = df_.get_view_by_idx<double,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 19:{
    auto view = df_.get_view_by_idx<float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 4:
  {
    switch(colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int,unsigned int,double,float>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int,unsigned int,double,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int,unsigned int,double,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int,unsigned int,float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int,unsigned int,float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto view = df_.get_view_by_idx<int,unsigned int,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto view = df_.get_view_by_idx<int,double,float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto view = df_.get_view_by_idx<int,double,float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto view = df_.get_view_by_idx<int,double,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto view = df_.get_view_by_idx<int,float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto view = df_.get_view_by_idx<unsigned int,double,float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto view = df_.get_view_by_idx<unsigned int,double,float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto view = df_.get_view_by_idx<unsigned int,double,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto view = df_.get_view_by_idx<unsigned int,float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto view = df_.get_view_by_idx<double,float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 5:
  {
    switch(colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int,unsigned int,double,float,signed char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto view = df_.get_view_by_idx<int,unsigned int,double,float,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto view = df_.get_view_by_idx<int,unsigned int,double,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto view = df_.get_view_by_idx<int,unsigned int,float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto view = df_.get_view_by_idx<int,double,float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto view = df_.get_view_by_idx<unsigned int,double,float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 6:
  {
    switch(colMagic_){
    case 0:{
    auto view = df_.get_view_by_idx<int,unsigned int,double,float,signed char,unsigned char>(slice);
    return DFView(std::move(view), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
}

    Util::die("invalid magic ({},{})", nDtypes_, colMagic_);
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