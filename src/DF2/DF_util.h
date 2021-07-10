#pragma once
#include "DF_def.h"

template<size_t N>
DF<N> DF_index(const DF<N>& in, const std::vector<int>& idxs)
{
    DF<N> out = Eigen::ArrayXXd::Zero(idxs.size(), N);
    for (size_t i = 0; i < idxs.size(); i++)
	{
		auto id = idxs[i];
        out.row(i) << in.row(id);
	}
    return out;
}