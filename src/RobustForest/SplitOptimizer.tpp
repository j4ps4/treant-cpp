#include <limits>

template<int N>
Eigen::Array<double,1,N> num_classes(const Eigen::ArrayXXd& y)
{
	return y.colwise().sum();
}

template<std::size_t N>
double SplitOptimizer<N>::logloss(const Eigen::ArrayXXd& y_true,
                   const Arr& y_pred)
{
    if (y_true.size() == 0)
        return 0.0;
    
    const auto y_pred_prime = y_pred.max(eps).min(1-eps).log();
    double accum = 0.0;
    for (size_t i = 0; i < y_true.rows(); i++)
	{
		accum -= (y_true.row(i) * y_pred_prime).sum();
	}
    return accum;
}

template<std::size_t N>
double SplitOptimizer<N>::logloss_under_attack(const Eigen::ArrayXXd& left,
                                       const Eigen::ArrayXXd& right,
                                       const Eigen::ArrayXXd& unknown,
                                       const Arr& pred) // length 2xN
{
    const Eigen::Array<double,1,N> left_p = pred.head<N>().max(eps).min(1-eps).log();
    const Eigen::Array<double,1,N> right_p = pred.tail<N>().max(eps).min(1-eps).log();
    const auto countsL = num_classes<N>(left);
    const auto countsR = num_classes<N>(right);
    const auto countsU = num_classes<N>(unknown);
    return -(countsL*left_p).sum() - (countsR*right_p).sum()
           + (countsU * ((-left_p).max(-right_p))).sum();
}

template<size_t N>
Eigen::Array<double,-1,N> array_index(const Eigen::ArrayXXd& in, const std::vector<int>& idxs)
{
    Eigen::Array<double,-1,N> out = Eigen::ArrayXXd::Zero(idxs.size(), N);
    for (size_t i = 0; i < idxs.size(); i++)
	{
		auto id = idxs[i];
        out.row(i) << in.row(id);
	}
    return out;
}

template<std::size_t N>
std::array<double,3> SplitOptimizer<N>::icml_split_loss(const Eigen::ArrayXXd& y,
    const std::vector<int>& L, const std::vector<int>& R)
{
    if (L.empty())
    {
        auto R_filt = array_index<N>(y, R);
        Arr pred_right = R_filt.colwise().mean();
    }
}