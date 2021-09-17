template<>
double eq_constr1<10>(unsigned n, const double* x, double* grad, void* data)
{
    if (grad != nullptr)
    {
grad[0] = 1.0;
grad[1] = 1.0;
grad[2] = 1.0;
grad[3] = 1.0;
grad[4] = 1.0;
grad[5] = 1.0;
grad[6] = 1.0;
grad[7] = 1.0;
grad[8] = 1.0;
grad[9] = 1.0;
grad[10] = 0.0;
grad[11] = 0.0;
grad[12] = 0.0;
grad[13] = 0.0;
grad[14] = 0.0;
grad[15] = 0.0;
grad[16] = 0.0;
grad[17] = 0.0;
grad[18] = 0.0;
grad[19] = 0.0;
}
return x[0]+x[1]+x[2]+x[3]+x[4]+x[5]+x[6]+x[7]+x[8]+x[9]-1.0;
}

template<>
double eq_constr2<10>(unsigned n, const double* x, double* grad, void* data)
{
    if (grad != nullptr)
    {
grad[0] = 0.0;
grad[1] = 0.0;
grad[2] = 0.0;
grad[3] = 0.0;
grad[4] = 0.0;
grad[5] = 0.0;
grad[6] = 0.0;
grad[7] = 0.0;
grad[8] = 0.0;
grad[9] = 0.0;
grad[10] = 1.0;
grad[11] = 1.0;
grad[12] = 1.0;
grad[13] = 1.0;
grad[14] = 1.0;
grad[15] = 1.0;
grad[16] = 1.0;
grad[17] = 1.0;
grad[18] = 1.0;
grad[19] = 1.0;
}
return x[10]+x[11]+x[12]+x[13]+x[14]+x[15]+x[16]+x[17]+x[18]+x[19]-1.0;
}
