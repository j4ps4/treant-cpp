
DF DF::get_data_by_idx(IdxT start, IdxT end) const
{
    auto idx = hmdf::Index2D<IdxT>{start, end};
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

DFR DF::get_row(IdxT idx) const
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
    auto row = df_.get_data_by_idx<int32_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto row = df_.get_data_by_idx<uint32_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto row = df_.get_data_by_idx<double>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto row = df_.get_data_by_idx<float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto row = df_.get_data_by_idx<int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto row = df_.get_data_by_idx<uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 2:
  {
    switch(this->colMagic_){
    case 0:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto row = df_.get_data_by_idx<int32_t,double>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto row = df_.get_data_by_idx<int32_t,float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto row = df_.get_data_by_idx<int32_t,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto row = df_.get_data_by_idx<int32_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto row = df_.get_data_by_idx<uint32_t,double>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto row = df_.get_data_by_idx<uint32_t,float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto row = df_.get_data_by_idx<uint32_t,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto row = df_.get_data_by_idx<uint32_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto row = df_.get_data_by_idx<double,float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto row = df_.get_data_by_idx<double,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto row = df_.get_data_by_idx<double,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto row = df_.get_data_by_idx<float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto row = df_.get_data_by_idx<float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto row = df_.get_data_by_idx<int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 3:
  {
    switch(this->colMagic_){
    case 0:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto row = df_.get_data_by_idx<int32_t,double,float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto row = df_.get_data_by_idx<int32_t,double,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto row = df_.get_data_by_idx<int32_t,double,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto row = df_.get_data_by_idx<int32_t,float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto row = df_.get_data_by_idx<int32_t,float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto row = df_.get_data_by_idx<int32_t,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto row = df_.get_data_by_idx<uint32_t,double,float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto row = df_.get_data_by_idx<uint32_t,double,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto row = df_.get_data_by_idx<uint32_t,double,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto row = df_.get_data_by_idx<uint32_t,float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto row = df_.get_data_by_idx<uint32_t,float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 15:{
    auto row = df_.get_data_by_idx<uint32_t,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 16:{
    auto row = df_.get_data_by_idx<double,float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 17:{
    auto row = df_.get_data_by_idx<double,float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 18:{
    auto row = df_.get_data_by_idx<double,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 19:{
    auto row = df_.get_data_by_idx<float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 4:
  {
    switch(this->colMagic_){
    case 0:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double,float>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 6:{
    auto row = df_.get_data_by_idx<int32_t,double,float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 7:{
    auto row = df_.get_data_by_idx<int32_t,double,float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 8:{
    auto row = df_.get_data_by_idx<int32_t,double,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 9:{
    auto row = df_.get_data_by_idx<int32_t,float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 10:{
    auto row = df_.get_data_by_idx<uint32_t,double,float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 11:{
    auto row = df_.get_data_by_idx<uint32_t,double,float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 12:{
    auto row = df_.get_data_by_idx<uint32_t,double,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 13:{
    auto row = df_.get_data_by_idx<uint32_t,float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 14:{
    auto row = df_.get_data_by_idx<double,float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 5:
  {
    switch(this->colMagic_){
    case 0:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 1:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double,float,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 2:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 3:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 4:{
    auto row = df_.get_data_by_idx<int32_t,double,float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    case 5:{
    auto row = df_.get_data_by_idx<uint32_t,double,float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
    }
    }
  }
  case 6:
  {
    switch(this->colMagic_){
    case 0:{
    auto row = df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t,uint8_t>(slice);
    return DFR(std::move(row), &colmap_, colMagic_, nDtypes_);
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
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<uint32_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<double>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 2:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 6:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 7:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 8:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 9:{
    auto df = parent_->df_.get_data_by_idx<double,float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 10:{
    auto df = parent_->df_.get_data_by_idx<double,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 11:{
    auto df = parent_->df_.get_data_by_idx<double,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 12:{
    auto df = parent_->df_.get_data_by_idx<float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 13:{
    auto df = parent_->df_.get_data_by_idx<float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 14:{
    auto df = parent_->df_.get_data_by_idx<int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 3:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 6:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 7:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 8:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 9:{
    auto df = parent_->df_.get_data_by_idx<int32_t,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 10:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 11:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 12:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 13:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 14:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 15:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 16:{
    auto df = parent_->df_.get_data_by_idx<double,float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 17:{
    auto df = parent_->df_.get_data_by_idx<double,float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 18:{
    auto df = parent_->df_.get_data_by_idx<double,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 19:{
    auto df = parent_->df_.get_data_by_idx<float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 4:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 6:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 7:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 8:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 9:{
    auto df = parent_->df_.get_data_by_idx<int32_t,float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 10:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 11:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 12:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 13:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 14:{
    auto df = parent_->df_.get_data_by_idx<double,float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 5:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 1:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 2:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 3:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 4:{
    auto df = parent_->df_.get_data_by_idx<int32_t,double,float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    case 5:{
    auto df = parent_->df_.get_data_by_idx<uint32_t,double,float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
  case 6:
  {
    switch(parent_->colMagic_){
    case 0:{
    auto df = parent_->df_.get_data_by_idx<int32_t,uint32_t,double,float,int8_t,uint8_t>(idx);
    return DFR(std::move(df), &(parent_->colmap_), parent_->colMagic_, parent_->nDtypes_);
    }
    }
  }
}
    auto s = fmt::format("invalid magic ({},{})", parent_->nDtypes_, parent_->colMagic_);
    throw std::runtime_error(s);
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