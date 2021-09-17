template<>
double logloss_nlopt<10>(unsigned n, const double* x, double* grad, void* data)
{
loss_data<10>* d = static_cast<loss_data<10>*>(data);
    const auto& cL = d->C_L;
    const auto& cU = d->C_U;
    const auto& cR = d->C_R;

auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};
const auto cc_0 = x[0] <= x[10] ? std::make_tuple(cL(0)+cU(0), cR(0)) 
                                   : std::make_tuple(cL(0), cR(0)+cU(0));
const auto cc_1 = x[1] <= x[11] ? std::make_tuple(cL(1)+cU(1), cR(1)) 
                                   : std::make_tuple(cL(1), cR(1)+cU(1));
const auto cc_2 = x[2] <= x[12] ? std::make_tuple(cL(2)+cU(2), cR(2)) 
                                   : std::make_tuple(cL(2), cR(2)+cU(2));
const auto cc_3 = x[3] <= x[13] ? std::make_tuple(cL(3)+cU(3), cR(3)) 
                                   : std::make_tuple(cL(3), cR(3)+cU(3));
const auto cc_4 = x[4] <= x[14] ? std::make_tuple(cL(4)+cU(4), cR(4)) 
                                   : std::make_tuple(cL(4), cR(4)+cU(4));
const auto cc_5 = x[5] <= x[15] ? std::make_tuple(cL(5)+cU(5), cR(5)) 
                                   : std::make_tuple(cL(5), cR(5)+cU(5));
const auto cc_6 = x[6] <= x[16] ? std::make_tuple(cL(6)+cU(6), cR(6)) 
                                   : std::make_tuple(cL(6), cR(6)+cU(6));
const auto cc_7 = x[7] <= x[17] ? std::make_tuple(cL(7)+cU(7), cR(7)) 
                                   : std::make_tuple(cL(7), cR(7)+cU(7));
const auto cc_8 = x[8] <= x[18] ? std::make_tuple(cL(8)+cU(8), cR(8)) 
                                   : std::make_tuple(cL(8), cR(8)+cU(8));
const auto cc_9 = x[9] <= x[19] ? std::make_tuple(cL(9)+cU(9), cR(9)) 
                                   : std::make_tuple(cL(9), cR(9)+cU(9));
if (grad != nullptr)
    {
grad[0] = std::get<0>(cc_0) * -1.0/x[0];
grad[1] = std::get<0>(cc_1) * -1.0/x[1];
grad[2] = std::get<0>(cc_2) * -1.0/x[2];
grad[3] = std::get<0>(cc_3) * -1.0/x[3];
grad[4] = std::get<0>(cc_4) * -1.0/x[4];
grad[5] = std::get<0>(cc_5) * -1.0/x[5];
grad[6] = std::get<0>(cc_6) * -1.0/x[6];
grad[7] = std::get<0>(cc_7) * -1.0/x[7];
grad[8] = std::get<0>(cc_8) * -1.0/x[8];
grad[9] = std::get<0>(cc_9) * -1.0/x[9];
grad[10] = std::get<1>(cc_0) * -1.0/x[10];
grad[11] = std::get<1>(cc_1) * -1.0/x[11];
grad[12] = std::get<1>(cc_2) * -1.0/x[12];
grad[13] = std::get<1>(cc_3) * -1.0/x[13];
grad[14] = std::get<1>(cc_4) * -1.0/x[14];
grad[15] = std::get<1>(cc_5) * -1.0/x[15];
grad[16] = std::get<1>(cc_6) * -1.0/x[16];
grad[17] = std::get<1>(cc_7) * -1.0/x[17];
grad[18] = std::get<1>(cc_8) * -1.0/x[18];
grad[19] = std::get<1>(cc_9) * -1.0/x[19];
}
return std::get<0>(cc_0) * -mlog(x[0])
 + std::get<0>(cc_1) * -mlog(x[1])
 + std::get<0>(cc_2) * -mlog(x[2])
 + std::get<0>(cc_3) * -mlog(x[3])
 + std::get<0>(cc_4) * -mlog(x[4])
 + std::get<0>(cc_5) * -mlog(x[5])
 + std::get<0>(cc_6) * -mlog(x[6])
 + std::get<0>(cc_7) * -mlog(x[7])
 + std::get<0>(cc_8) * -mlog(x[8])
 + std::get<0>(cc_9) * -mlog(x[9])
 + std::get<1>(cc_0) * -mlog(x[10])
 + std::get<1>(cc_1) * -mlog(x[11])
 + std::get<1>(cc_2) * -mlog(x[12])
 + std::get<1>(cc_3) * -mlog(x[13])
 + std::get<1>(cc_4) * -mlog(x[14])
 + std::get<1>(cc_5) * -mlog(x[15])
 + std::get<1>(cc_6) * -mlog(x[16])
 + std::get<1>(cc_7) * -mlog(x[17])
 + std::get<1>(cc_8) * -mlog(x[18])
 + std::get<1>(cc_9) * -mlog(x[19]);
}