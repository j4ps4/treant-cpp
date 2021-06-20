#include <limits>
#include <algorithm>

template<int N>
Eigen::Array<double,1,N> num_classes(const Eigen::ArrayXXd& y)
{
	return y.colwise().sum();
}

template<std::size_t N>
double SplitOptimizer<N>::sse(const Eigen::ArrayXXd& y_true,
                   const Arr& y_pred)
{
    if (y_true.size() == 0)
        return 0.0;

    double accum = 0.0;
    for (size_t i = 0; i < y_true.rows(); i++)
	{
		accum += (y_true.row(i) - y_pred).pow(2.0).sum();
	}
    return accum;
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
double SplitOptimizer<N>::icml_split_loss(const Eigen::ArrayXXd& y,
    const std::vector<int>& L, const std::vector<int>& R)
{
    if (L.empty())
    {
        auto R_filt = array_index<N>(y, R);
        Arr pred_right = R_filt.colwise().mean();
        double icml_loss = sse(R_filt, pred_right);
        return icml_loss;
    }
    else if (R.empty())
    {
        auto L_filt = array_index<N>(y, L);
        Arr pred_left = L_filt.colwise().mean();
        double icml_loss = sse(L_filt, pred_left);
        return icml_loss;
    }
    else
    {
        auto L_filt = array_index<N>(y, L);
        Arr pred_left = L_filt.colwise().mean();
        auto R_filt = array_index<N>(y, R);
        Arr pred_right = R_filt.colwise().mean();
        double icml_loss = sse(L_filt, pred_left) + sse(R_filt, pred_right);
        return icml_loss;
    }
}

template<size_t NY>
template<size_t NA>
auto SplitOptimizer<NY>::split_icml2019(
    const Eigen::ArrayXXd& X, const Eigen::ArrayXXd& y, const IdxVec& rows, Attacker<NA>& attacker,
    std::vector<int>& costs, size_t feature_id, double feature_value) 
    -> std::tuple<IdxVec, IdxVec, IdxVec, std::optional<IcmlTupl>>
{
    //indices of instances which surely DO satisfy the boolean spitting predicate, disregarding the attacker
    IdxVec split_left;
    IdxVec split_right;
    // indices of instances which may or may not satisfy the boolean splitting predicate
    IdxVec split_unknown_left;
    IdxVec split_unknown_right;

    for (auto row_id : rows)
    {
        int cost = costs[row_id]; // get the i-th cost spent on the i-th instance so far
        auto attacks = attacker.attack(X.row(row_id), feature_id, cost);
        bool all_left = true;
        bool all_right = true;
        for (auto& [inst, c]: attacks)
        {
            if (inst[feature_id] <= feature_value)
                all_right = false;
            else
                all_left = false;

            if (!all_left && !all_right)
                break;
        }

        if (all_left)
        {
            // it means the splitting predicate is ALWAYS satisfied by this instance, no matter what the attacker does
            // as such, we can safely place this instance among those which surely go to the true (left) branch
            split_left.push_back(row_id);
        }
        else if (all_right)
        {
            // it means the splitting predicate is NEVER satisfied by this instance, no matter what the attacker does
            split_right.push_back(row_id);
        }
        else
        {
            if (X.row(row_id)[feature_id] <= feature_value)
                split_unknown_left.push_back(row_id);
            else
                split_unknown_right.push_back(row_id);
        }
    }
    std::vector<double> icml_options;
    IdxVec icml_left;
    IdxVec icml_right;

    // case 1: no perturbations
    icml_left.reserve(split_left.size()+split_unknown_left.size());
    icml_left.insert(icml_left.end(), split_left.begin(), split_left.end());
    icml_left.insert(icml_left.end(), split_unknown_left.begin(), split_unknown_left.end());
    icml_right.reserve(split_right.size()+split_unknown_right.size());
    icml_right.insert(icml_right.end(), split_right.begin(), split_right.end());
    icml_right.insert(icml_right.end(), split_unknown_right.begin(), split_unknown_right.end());
    icml_options.push_back(icml_split_loss(y, icml_left, icml_right));

    // case 2: swap
    icml_left.clear();
    icml_left.insert(icml_left.end(), split_left.begin(), split_left.end());
    icml_left.insert(icml_left.end(), split_unknown_right.begin(), split_unknown_right.end());
    icml_right.clear();
    icml_right.insert(icml_right.end(), split_right.begin(), split_right.end());
    icml_right.insert(icml_right.end(), split_unknown_left.begin(), split_unknown_left.end());
    icml_options.push_back(icml_split_loss(y, icml_left, icml_right));

    // case 3: all left
    icml_left.insert(icml_left.end(), split_unknown_left.begin(), split_unknown_left.end());
    icml_right.clear();
    icml_right.insert(icml_right.end(), split_right.begin(), split_right.end());
    icml_options.push_back(icml_split_loss(y, icml_left, icml_right));

    // case 4: all right
    icml_right.insert(icml_right.end(), split_unknown_right.begin(), split_unknown_right.end());
    icml_right.insert(icml_right.end(), split_unknown_left.begin(), split_unknown_left.end());
    icml_left.clear();
    icml_left.insert(icml_left.end(), split_left.begin(), split_left.end());
    icml_options.push_back(icml_split_loss(y, icml_left, icml_right));

    if (icml_options.empty())
    {
        IdxVec tmp(split_unknown_right.begin(), split_unknown_right.end());
        tmp.insert(tmp.end(), split_unknown_left.begin(), split_unknown_left.end());
        return {split_left, split_right, tmp, {}};
    }
    else if (split_left.size()+split_unknown_left.size()==0 ||
             split_right.size()+split_unknown_right.size()==0)
    {
        IdxVec tmp(split_unknown_right.begin(), split_unknown_right.end());
        tmp.insert(tmp.end(), split_unknown_left.begin(), split_unknown_left.end());
        return {split_left, split_right, tmp, {}};
    }
    else
    {
        auto max_it = std::max_element(icml_options.begin(), icml_options.end());
        auto sse = *max_it;
        IdxVec tmpLeft(split_left.begin(), split_left.end());
        tmpLeft.insert(tmpLeft.end(), split_unknown_left.begin(), split_unknown_left.end());
        IdxVec tmpRight(split_right.begin(), split_right.end());
        tmpRight.insert(tmpRight.end(), split_unknown_right.begin(), split_unknown_right.end());
        auto y_pred_left = array_index<NY>(y, tmpLeft).colwise().mean();
        auto y_pred_right = array_index<NY>(y, tmpRight).colwise().mean();
        IdxVec tmp(split_unknown_right.begin(), split_unknown_right.end());
        tmp.insert(tmp.end(), split_unknown_left.begin(), split_unknown_left.end());
        return {split_left, split_right, tmp, std::make_tuple(y_pred_left, y_pred_right, sse)};
    }
}

template<size_t NY>
template<size_t NA>
auto SplitOptimizer<NY>::simulate_split(
    const Eigen::ArrayXXd& X, const IdxVec& rows, Attacker<NA>& attacker,
    std::vector<int>& costs, size_t feature_id, double feature_value
) -> std::tuple<IdxVec, IdxVec, IdxVec>
{
    IdxVec split_left;
    IdxVec split_right;
    IdxVec split_unknown;

    for (auto row_id : rows)
    {
        int cost = costs[row_id];
        auto attacks = attacker.attack(X.row(row_id), feature_id, cost);

        bool all_left = true;
        bool all_right = true;

        for (auto& [inst, c]: attacks)
        {
            if (inst[feature_id] <= feature_value)
                all_right = false;
            else
                all_left = false;
            
            if (!all_left && !all_right)
                break;
        }

        if (all_left)
        {
            split_left.push_back(row_id);
        }
        else if (all_right)
        {
            split_right.push_back(row_id);
        }
        else
        {
            split_unknown.push_back(row_id);
        }
    }
    return {split_left, split_right, split_unknown};
}