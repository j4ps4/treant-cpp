#include <limits>
#include <algorithm>
#include <ranges>

#include <mutex>

#include "../DF2/DF_util.h"
#include "../thread_pool.hpp"

std::mutex dbg_mut;

template<size_t NX, size_t NY>
double SplitOptimizer<NX,NY>::sse(const DF<NY>& y_true,
                   const Row<NY>& y_pred)
{
    if (y_true.size() == 0)
        return 0.0;

    double accum = 0.0;
    for (int64_t i = 0; i < y_true.rows(); i++)
	{
		accum += (y_true.row(i) - y_pred).pow(2.0).sum();
	}
    return accum;
}

template<size_t NX, size_t NY>
double SplitOptimizer<NX,NY>::logloss(const DF<NY>& y_true,
                   const Row<NY>& y_pred)
{
    // Util::log("logloss: y_true: ({},{})", y_true.rows(), y_true.cols());
    // row_printf<NY>("y_pred:", y_pred);
    if (y_true.rows() == 0)
        return 0.0;
    
    const auto y_pred_prime = y_pred.max(EPS).min(1-EPS).log();
    double accum = 0.0;
    for (int64_t i = 0; i < y_true.rows(); i++)
	{
		accum -= (y_true.row(i) * y_pred_prime).sum();
	}
    return accum;
}

template<size_t NX, size_t NY>
Eigen::ArrayXd SplitOptimizer<NX,NY>::instance_logloss(const DF<NY>& y_true,
    const Row<NY>& y_pred)
{
    Eigen::ArrayXd out(y_true.rows(), 1);

    const auto y_pred_prime = y_pred.max(EPS).min(1-EPS).log();
    for (int64_t i = 0; i < y_true.rows(); i++)
	{
		out(i) = -((y_true.row(i) * y_pred_prime).sum());
	}
    return out;
}

template<size_t NY>
struct loss_data
{
    const Row<NY> C_L;
    const Row<NY> C_U;
    const Row<NY> C_R;
};


// loss function for nlopt to minimize
template<size_t NY>
static double logloss_nlopt(unsigned n, const double* x, double* grad, void* data)
{
    Util::die("logloss_nlopt not implemented for this N");
}

template<>
double logloss_nlopt<2>(unsigned n, const double* x, double* grad, void* data)
{
    loss_data<2>* d = static_cast<loss_data<2>*>(data);
    const auto& cL = d->C_L;
    const auto& cU = d->C_U;
    const auto& cR = d->C_R;

    // safe log
    auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};

    // push class-0 unknowns to which branch?
    const auto cc_0 = x[0] <= x[2] ? std::make_tuple(cL(0)+cU(0), cR(0)) 
                                   : std::make_tuple(cL(0), cR(0)+cU(0));
    // push class-1 unknowns to which branch?
    const auto cc_1 = x[1] <= x[3] ? std::make_tuple(cL(1)+cU(1), cR(1)) 
                                   : std::make_tuple(cL(1), cR(1)+cU(1));
    
    // const auto sL = std::get<0>(cc_0)+std::get<0>(cc_1);
    // const auto sR = std::get<1>(cc_0)+std::get<1>(cc_1);

    if (grad != nullptr)
    {
        grad[0] = std::get<0>(cc_0) * -1.0/std::max(EPS,x[0]);
        grad[1] = std::get<0>(cc_1) * -1.0/std::max(EPS,x[1]);
        grad[2] = std::get<1>(cc_0) * -1.0/std::max(EPS,x[2]);
        grad[3] = std::get<1>(cc_1) * -1.0/std::max(EPS,x[3]);
    }
    return std::get<0>(cc_0) * -mlog(x[0])
         + std::get<0>(cc_1) * -mlog(x[1])
         + std::get<1>(cc_0) * -mlog(x[2])
         + std::get<1>(cc_1) * -mlog(x[3]);

}

template<>
double logloss_nlopt<6>(unsigned n, const double* x, double* grad, void* data)
{
    // if (!check_x<12>(x))
    // {
    //     fmt::print("ERROR: faulty x: ");
    //     print_x<12>(x);
    //     // std::exit(1);
    // }
    loss_data<6>* d = static_cast<loss_data<6>*>(data);
    const auto& cL = d->C_L;
    const auto& cU = d->C_U;
    const auto& cR = d->C_R;

    // safe log
    auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};

    // push class-0 unknowns to which branch?
    const auto cc_0 = x[0] <= x[6] ? std::make_tuple(cL(0)+cU(0), cR(0)) 
                                   : std::make_tuple(cL(0), cR(0)+cU(0));
    // push class-1 unknowns to which branch?
    const auto cc_1 = x[1] <= x[7] ? std::make_tuple(cL(1)+cU(1), cR(1)) 
                                   : std::make_tuple(cL(1), cR(1)+cU(1));
    // push class-2 unknowns to which branch?
    const auto cc_2 = x[2] <= x[8] ? std::make_tuple(cL(2)+cU(2), cR(2)) 
                                   : std::make_tuple(cL(2), cR(2)+cU(2));
    // push class-3 unknowns to which branch?
    const auto cc_3 = x[3] <= x[9] ? std::make_tuple(cL(3)+cU(3), cR(3)) 
                                   : std::make_tuple(cL(3), cR(3)+cU(3));
    // push class-4 unknowns to which branch?
    const auto cc_4 = x[4] <= x[10] ? std::make_tuple(cL(4)+cU(4), cR(4)) 
                                    : std::make_tuple(cL(4), cR(4)+cU(4));
    // push class-5 unknowns to which branch?
    const auto cc_5 = x[5] <= x[11] ? std::make_tuple(cL(5)+cU(5), cR(5)) 
                                    : std::make_tuple(cL(5), cR(5)+cU(5));

    if (grad != nullptr)
    {
        grad[0] = std::get<0>(cc_0) * -1.0/std::max(EPS,x[0]);
        grad[1] = std::get<0>(cc_1) * -1.0/std::max(EPS,x[1]);
        grad[2] = std::get<0>(cc_2) * -1.0/std::max(EPS,x[2]);
        grad[3] = std::get<0>(cc_3) * -1.0/std::max(EPS,x[3]);
        grad[4] = std::get<0>(cc_4) * -1.0/std::max(EPS,x[4]);
        grad[5] = std::get<0>(cc_5) * -1.0/std::max(EPS,x[5]);
        grad[6] = std::get<1>(cc_0) * -1.0/std::max(EPS,x[6]);
        grad[7] = std::get<1>(cc_1) * -1.0/std::max(EPS,x[7]);
        grad[8] = std::get<1>(cc_2) * -1.0/std::max(EPS,x[8]);
        grad[9] = std::get<1>(cc_3) * -1.0/std::max(EPS,x[9]);
        grad[10] = std::get<1>(cc_4) * -1.0/std::max(EPS,x[10]);
        grad[11] = std::get<1>(cc_5) * -1.0/std::max(EPS,x[11]);
    }
    return std::get<0>(cc_0) * -mlog(x[0])
         + std::get<0>(cc_1) * -mlog(x[1])
         + std::get<0>(cc_2) * -mlog(x[2])
         + std::get<0>(cc_3) * -mlog(x[3])
         + std::get<0>(cc_4) * -mlog(x[4])
         + std::get<0>(cc_5) * -mlog(x[5])
         + std::get<1>(cc_0) * -mlog(x[6])
         + std::get<1>(cc_1) * -mlog(x[7])
         + std::get<1>(cc_2) * -mlog(x[8])
         + std::get<1>(cc_3) * -mlog(x[9])
         + std::get<1>(cc_4) * -mlog(x[10])
         + std::get<1>(cc_5) * -mlog(x[11]);

}

template<>
double logloss_nlopt<7>(unsigned n, const double* x, double* grad, void* data)
{
    loss_data<7>* d = static_cast<loss_data<7>*>(data);
    const auto& cL = d->C_L;
    const auto& cU = d->C_U;
    const auto& cR = d->C_R;

    // safe log
    auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};

    // push class-0 unknowns to which branch?
    const auto cc_0 = x[0] <= x[7] ? std::make_tuple(cL(0)+cU(0), cR(0)) 
                                   : std::make_tuple(cL(0), cR(0)+cU(0));
    // push class-1 unknowns to which branch?
    const auto cc_1 = x[1] <= x[8] ? std::make_tuple(cL(1)+cU(1), cR(1)) 
                                   : std::make_tuple(cL(1), cR(1)+cU(1));
    // push class-2 unknowns to which branch?
    const auto cc_2 = x[2] <= x[9] ? std::make_tuple(cL(2)+cU(2), cR(2)) 
                                   : std::make_tuple(cL(2), cR(2)+cU(2));
    // push class-3 unknowns to which branch?
    const auto cc_3 = x[3] <= x[10] ? std::make_tuple(cL(3)+cU(3), cR(3)) 
                                   : std::make_tuple(cL(3), cR(3)+cU(3));
    // push class-4 unknowns to which branch?
    const auto cc_4 = x[4] <= x[11] ? std::make_tuple(cL(4)+cU(4), cR(4)) 
                                    : std::make_tuple(cL(4), cR(4)+cU(4));
    // push class-5 unknowns to which branch?
    const auto cc_5 = x[5] <= x[12] ? std::make_tuple(cL(5)+cU(5), cR(5)) 
                                    : std::make_tuple(cL(5), cR(5)+cU(5));
    // push class-6 unknowns to which branch?
    const auto cc_6 = x[6] <= x[13] ? std::make_tuple(cL(6)+cU(6), cR(6)) 
                                    : std::make_tuple(cL(6), cR(6)+cU(6));

    if (grad != nullptr)
    {
        grad[0] = std::get<0>(cc_0) * -1.0/std::max(EPS,x[0]);
        grad[1] = std::get<0>(cc_1) * -1.0/std::max(EPS,x[1]);
        grad[2] = std::get<0>(cc_2) * -1.0/std::max(EPS,x[2]);
        grad[3] = std::get<0>(cc_3) * -1.0/std::max(EPS,x[3]);
        grad[4] = std::get<0>(cc_4) * -1.0/std::max(EPS,x[4]);
        grad[5] = std::get<0>(cc_5) * -1.0/std::max(EPS,x[5]);
        grad[6] = std::get<0>(cc_6) * -1.0/std::max(EPS,x[6]);
        grad[7] = std::get<1>(cc_0) * -1.0/std::max(EPS,x[7]);
        grad[8] = std::get<1>(cc_1) * -1.0/std::max(EPS,x[8]);
        grad[9] = std::get<1>(cc_2) * -1.0/std::max(EPS,x[9]);
        grad[10] = std::get<1>(cc_3) * -1.0/std::max(EPS,x[10]);
        grad[11] = std::get<1>(cc_4) * -1.0/std::max(EPS,x[11]);
        grad[12] = std::get<1>(cc_5) * -1.0/std::max(EPS,x[12]);
        grad[13] = std::get<1>(cc_6) * -1.0/std::max(EPS,x[13]);
    }
    return std::get<0>(cc_0) * -mlog(x[0])
         + std::get<0>(cc_1) * -mlog(x[1])
         + std::get<0>(cc_2) * -mlog(x[2])
         + std::get<0>(cc_3) * -mlog(x[3])
         + std::get<0>(cc_4) * -mlog(x[4])
         + std::get<0>(cc_5) * -mlog(x[5])
         + std::get<0>(cc_6) * -mlog(x[6])
         + std::get<1>(cc_0) * -mlog(x[7])
         + std::get<1>(cc_1) * -mlog(x[8])
         + std::get<1>(cc_2) * -mlog(x[9])
         + std::get<1>(cc_3) * -mlog(x[10])
         + std::get<1>(cc_4) * -mlog(x[11])
         + std::get<1>(cc_5) * -mlog(x[12])
         + std::get<1>(cc_6) * -mlog(x[13]);

}

#include "logloss_10.tpp"

// Equality constraints: lower and upper halves of x must each sum to 1
template<size_t NY>
static double eq_constr1(unsigned n, const double* x, double* grad, void* data) {return 0.0;};

template<>
double eq_constr1<2>(unsigned n, const double* x, double* grad, void* data)
{
    if (grad != nullptr)
    {
        grad[0] = 1.0;
        grad[1] = 1.0;
        grad[2] = 0.0;
        grad[3] = 0.0;
    }
    return x[0] + x[1] - 1.0;
}

template<>
double eq_constr1<6>(unsigned n, const double* x, double* grad, void* data)
{
    if (grad != nullptr)
    {
        grad[0] = 1.0;
        grad[1] = 1.0;
        grad[2] = 1.0;
        grad[3] = 1.0;
        grad[4] = 1.0;
        grad[5] = 1.0;
        grad[6] = 0.0;
        grad[7] = 0.0;
        grad[8] = 0.0;
        grad[9] = 0.0;
        grad[10] = 0.0;
        grad[11] = 0.0;
    }
    return x[0] + x[1] + x[2] + x[3] + x[4] + x[5] - 1.0;
}

template<>
double eq_constr1<7>(unsigned n, const double* x, double* grad, void* data)
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
        grad[7] = 0.0;
        grad[8] = 0.0;
        grad[9] = 0.0;
        grad[10] = 0.0;
        grad[11] = 0.0;
        grad[12] = 0.0;
        grad[13] = 0.0;
    }
    return x[0] + x[1] + x[2] + x[3] + x[4] + x[5] + x[6] - 1.0;
}

template<size_t NY>
static double eq_constr2(unsigned n, const double* x, double* grad, void* data) {return 0.0;};

template<>
double eq_constr2<2>(unsigned n, const double* x, double* grad, void* data)
{
    if (grad != nullptr)
    {
        grad[0] = 0.0;
        grad[1] = 0.0;
        grad[2] = 1.0;
        grad[3] = 1.0;
    }
    return x[2] + x[3] - 1.0;
}

template<>
double eq_constr2<6>(unsigned n, const double* x, double* grad, void* data)
{
    if (grad != nullptr)
    {
        grad[0] = 0.0;
        grad[1] = 0.0;
        grad[2] = 0.0;
        grad[3] = 0.0;
        grad[4] = 0.0;
        grad[5] = 0.0;
        grad[6] = 1.0;
        grad[7] = 1.0;
        grad[8] = 1.0;
        grad[9] = 1.0;
        grad[10] = 1.0;
        grad[11] = 1.0;
    }
    return x[6] + x[7] + x[8] + x[9] + x[10] + x[11] - 1.0;
}

template<>
double eq_constr2<7>(unsigned n, const double* x, double* grad, void* data)
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
        grad[7] = 1.0;
        grad[8] = 1.0;
        grad[9] = 1.0;
        grad[10] = 1.0;
        grad[11] = 1.0;
        grad[12] = 1.0;
        grad[13] = 1.0;
    }
    return x[7] + x[8] + x[9] + x[10] + x[11] + x[12] + x[13] - 1.0;
}

#include "eq_constraint_10.tpp"

template<size_t NX, size_t NY>
double SplitOptimizer<NX,NY>::logloss_under_attack(const DF<NY>& left,
                                       const DF<NY>& right,
                                       const DF<NY>& unknown,
                                       const Row<NY2>& pred)
{
    //constexpr size_t H = NY/2;
    const Row<NY> left_p = pred.template head<NY>().max(EPS).min(1-EPS).log();
    const Row<NY> right_p = pred.template tail<NY>().max(EPS).min(1-EPS).log();
    const auto countsL = num_classes<NY>(left);
    const auto countsR = num_classes<NY>(right);
    const auto countsU = num_classes<NY>(unknown);
    return -(countsL*left_p).sum() - (countsR*right_p).sum()
           + (countsU * ((-left_p).max(-right_p))).sum();
}

template<size_t NX, size_t NY>
double SplitOptimizer<NX,NY>::evaluate_split(const DF<NY>& y_true,
                          const Row<NY>& y_pred) const
{
    // Use logloss
    // if (split_ == SplitFunction::LogLoss)
        return logloss(y_true, y_pred);
    // else
    //     return sse(y_true, y_pred);
}

template<size_t NX, size_t NY>
double SplitOptimizer<NX,NY>::split_loss(const DF<NY>& L, const NRow& pred_left,
    const DF<NY>& R, const NRow& pred_right) const
{
    if (L.rows() == 0)
    {
        // auto R_filt = DF_index<NY>(y, R);
        // NRow pred_right = R_filt.colwise().mean();
        double icml_loss = evaluate_split(R, pred_right);
        return icml_loss;
    }
    else if (R.rows() == 0)
    {
        // auto L_filt = DF_index<NY>(y, L);
        // NRow pred_left = L_filt.colwise().mean();
        double icml_loss = evaluate_split(L, pred_left);
        return icml_loss;
    }
    else
    {
        // auto L_filt = DF_index<NY>(y, L);
        // NRow pred_left = L_filt.colwise().mean();
        // auto R_filt = DF_index<NY>(y, R);
        // NRow pred_right = R_filt.colwise().mean();
        double icml_loss = evaluate_split(L, pred_left) + evaluate_split(R, pred_right);
        return icml_loss;
    }
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::split_icml2019(
    const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, Attacker<NX>& attacker,
    const CostMap& costs, size_t feature_id, double feature_value) const
    -> std::tuple<IdxVec, IdxVec, IdxVec, std::optional<IcmlTupl>>
{
    //indices of instances which surely DO satisfy the boolean spitting predicate, disregarding the attacker
    IdxVec split_left;
    IdxVec split_right;
    // indices of instances which may or may not satisfy the boolean splitting predicate
    IdxVec split_unknown_left;
    IdxVec split_unknown_right;

    const double epsilon = epsilon_ * epsilonCoeff_[feature_id];
    const bool perturb = epsilon > 0.0 && 
        (chen_perturb_ids_.empty() || chen_perturb_ids_.contains(feature_id));

    for (auto row_id : rows)
    {
        if (perturb)
        {
            const auto& row = X.row(row_id);
            const auto jth_feat = row[feature_id];
            if (jth_feat < feature_value - epsilon)
                split_left.push_back(row_id);
            else if (jth_feat > feature_value + epsilon)
                split_right.push_back(row_id);
            else if (jth_feat >= feature_value - epsilon && jth_feat < feature_value)
                    split_unknown_left.push_back(row_id);
            else if (jth_feat >= feature_value && jth_feat <= feature_value + epsilon)
                    split_unknown_right.push_back(row_id);
            else
                throw std::runtime_error("split_icml2019 failure");
        }
        else
        {
            const auto& row = X.row(row_id);
            const auto jth_feat = row[feature_id];
            if (jth_feat <= feature_value)
                split_left.push_back(row_id);
            else
                split_right.push_back(row_id);
        }
    }

    if (!perturb)
    {
        auto L = DF_index<NY>(y, split_left);
        auto R = DF_index<NY>(y, split_right);
        auto pred_left = null_pred<NY>(L);
        auto pred_right = null_pred<NY>(R);
        auto loss = split_loss(L, pred_left, R, pred_right);
        return {split_left, split_right, {}, std::make_tuple(pred_left, pred_right, loss)};
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
    auto L = DF_index<NY>(y, icml_left);
    auto R = DF_index<NY>(y, icml_right);
    {
        auto pred_left = null_pred<NY>(L);
        auto pred_right = null_pred<NY>(R);
        icml_options.push_back(split_loss(L, pred_left, R, pred_right));
    }

    // case 2: swap
    icml_left.clear();
    icml_left.insert(icml_left.end(), split_left.begin(), split_left.end());
    icml_left.insert(icml_left.end(), split_unknown_right.begin(), split_unknown_right.end());
    icml_right.clear();
    icml_right.insert(icml_right.end(), split_right.begin(), split_right.end());
    icml_right.insert(icml_right.end(), split_unknown_left.begin(), split_unknown_left.end());
    L = DF_index<NY>(y, icml_left);
    R = DF_index<NY>(y, icml_right);
    {
        auto pred_left = null_pred<NY>(L);
        auto pred_right = null_pred<NY>(R);
        icml_options.push_back(split_loss(L, pred_left, R, pred_right));
    }

    // case 3: all left
    icml_left.insert(icml_left.end(), split_unknown_left.begin(), split_unknown_left.end());
    icml_right.clear();
    icml_right.insert(icml_right.end(), split_right.begin(), split_right.end());
    L = DF_index<NY>(y, icml_left);
    R = DF_index<NY>(y, icml_right);
    {
        auto pred_left = null_pred<NY>(L);
        auto pred_right = null_pred<NY>(R);
        icml_options.push_back(split_loss(L, pred_left, R, pred_right));
    }

    // case 4: all right
    icml_right.insert(icml_right.end(), split_unknown_right.begin(), split_unknown_right.end());
    icml_right.insert(icml_right.end(), split_unknown_left.begin(), split_unknown_left.end());
    icml_left.clear();
    icml_left.insert(icml_left.end(), split_left.begin(), split_left.end());
    L = DF_index<NY>(y, icml_left);
    R = DF_index<NY>(y, icml_right);
    {
        auto pred_left = null_pred<NY>(L);
        auto pred_right = null_pred<NY>(R);
        icml_options.push_back(split_loss(L, pred_left, R, pred_right));
    }

    // if (icml_options.empty())
    // {
    //     IdxVec tmp(split_unknown_right.begin(), split_unknown_right.end());
    //     tmp.insert(tmp.end(), split_unknown_left.begin(), split_unknown_left.end());
    //     return {split_left, split_right, tmp, {}};
    // }
    if (split_left.size()+split_unknown_left.size()==0 ||
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
        auto tmpDFleft = DF_index<NY>(y, tmpLeft);
        auto tmpDFright = DF_index<NY>(y, tmpRight);
        auto y_pred_left = null_pred<NY>(tmpDFleft);
        auto y_pred_right = null_pred<NY>(tmpDFright);
        IdxVec tmp(split_unknown_right.begin(), split_unknown_right.end());
        tmp.insert(tmp.end(), split_unknown_left.begin(), split_unknown_left.end());
        return {split_left, split_right, tmp, std::make_tuple(y_pred_left, y_pred_right, sse)};
    }
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::simple_split(
    const DF<NX>& X, const DF<NY>& y, const IdxVec& rows,
    size_t feature_id, double feature_value) const
    -> std::tuple<IdxVec, IdxVec, std::optional<IcmlTupl>>
{
    IdxVec split_left;
    IdxVec split_right;

    for (auto row_id : rows)
    {
        const auto& inst = X.row(row_id);
        if (inst[feature_id] <= feature_value)
            split_left.push_back(row_id);
        else
            split_right.push_back(row_id);
    }
    auto L = DF_index<NY>(y, split_left);
    auto R = DF_index<NY>(y, split_right);
    auto pred_left = null_pred<NY>(L);
    auto pred_right = null_pred<NY>(R);
    auto loss = split_loss(L, pred_left, R, pred_right);
    return {split_left, split_right, std::make_tuple(pred_left, pred_right, loss)};
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::simulate_split(
    const DF<NX>& X, const IdxVec& rows, Attacker<NX>& attacker,
    const CostMap& costs, size_t feature_id, double feature_value) const
    -> std::tuple<IdxVec, IdxVec, IdxVec>
{
    IdxVec split_left;
    IdxVec split_right;
    IdxVec split_unknown;

    const double deform = attacker.get_deformation();
    const int budget = attacker.get_budget();

    for (auto row_id : rows)
    {
    	// get the cost spent on the i-th instance so far
        int cost = costs.at(row_id);
        bool all_left = true;
        bool all_right = true;

        if (attacker.is_flat())
        {
            const auto& row = X.row(row_id);
            const auto jth_feat = row[feature_id];
            if (jth_feat <= feature_value)
                all_right = false;
            else
                all_left = false;

            int multipl = 1;
            while (cost < budget)
            {
                if (jth_feat - multipl*deform <= feature_value)
                    all_right = false;
                else
                    all_left = false;
                if (!all_left && !all_right)
                    break;
                
                if (jth_feat + multipl*deform <= feature_value)
                    all_right = false;
                else
                    all_left = false;
                if (!all_left && !all_right)
                    break;
                multipl++;
                cost++;
            }
        }
        else
        {
            // collect all the attacks the attacker can do on the i-th instance
            auto attacks = attacker.attack(X.row(row_id), feature_id, cost);

            for (auto& [inst, c]: attacks)
            {
                if (inst[feature_id] <= feature_value)
                    all_right = false;
                else
                    all_left = false;
                
                if (!all_left && !all_right)
                    break;
            }
        }

		// it means the splitting predicate is ALWAYS satisfied by this instance, no matter what the attacker does
        if (all_left)
        {
            split_left.push_back(row_id);
        }
        // it means the splitting predicate is NEVER satisfied by this instance, no matter what the attacker does
        else if (all_right)
        {
            split_right.push_back(row_id);
        }
        // it means the splitting predicate MAY or MAY NOT be satisfied, depending on what the attacker does
        else
        {
            split_unknown.push_back(row_id);
        }
    }
    return {split_left, split_right, split_unknown};
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::optimize_loss_under_attack(
    const DF<NY>& y, const Row<NY>& current_prediction_score,
    const IdxVec& split_left, const IdxVec& split_right, 
    const IdxVec& split_unknown, FunVec& constraints, 
    std::vector<Constr_data<NY>>& constr_data) const -> std::optional<IcmlTupl>
{
    if (split_unknown.empty() && constraints.empty())
    {
        auto L = DF_index<NY>(y, split_left);
        auto R = DF_index<NY>(y, split_right);
        auto pred_left = null_pred<NY>(L);
        auto pred_right = null_pred<NY>(R);
        auto loss = split_loss(L, pred_left, R, pred_right);
        return IcmlTupl{pred_left, pred_right, loss};
    }

    auto L = DF_index<NY>(y, split_left);
    auto U = DF_index<NY>(y, split_unknown);
    auto R = DF_index<NY>(y, split_right);
    auto CL = num_classes<NY>(L);
    auto CU = num_classes<NY>(U);
    auto CR = num_classes<NY>(R);

    // seed
    std::vector<double> x(NY2);
    // std::vector<double> x_0(NY2);
    for (size_t i = 0; i < NY; i++)
    {
        x[i] = current_prediction_score(i);
        x[i+NY] = current_prediction_score(i);
        // x_0[i] = current_prediction_score(i);
        // x_0[i+NY] = current_prediction_score(i);
    }

    double minf;
    try
    {
        // set up nlopt
        nlopt::opt optimizer(optim_algo_, NY2);
        optimizer.set_lower_bounds(0);
        optimizer.set_upper_bounds(1);
        double tol = 1e-10;
        optimizer.set_xtol_rel(tol);
        optimizer.set_maxeval(maxiter_);
        loss_data<NY> d = {CL, CU, CR};
        // if (split_ == SplitFunction::LogLoss)
            optimizer.set_min_objective(logloss_nlopt<NY>, &d);
        // else
        //     throw std::runtime_error("not implemented");

        // if (constraints.size() > 0)
        //     fmt::print("using {} constraints.\n", constraints.size());
        for (size_t i = 0; i < constraints.size(); i++)
        {
            auto fun = constraints[i];
            auto c_data = &constr_data[i];
            optimizer.add_inequality_constraint(*(fun.target<double(*)(unsigned,const double*,double*,void*)>()), c_data, 1e-8);
        }
        optimizer.add_equality_constraint(eq_constr1<NY>, nullptr, 1e-8);
        optimizer.add_equality_constraint(eq_constr2<NY>, nullptr, 1e-8);

        auto result = optimizer.optimize(x, minf);
    }
    catch(nlopt::roundoff_limited&)
    {
        // NLOPT documentation states that it's safe to return final value in this case
    }
    catch(std::exception& e)
    {
        // Util::warn("caught NLOPT exception: {}", e.what());
        // if (!constraints.empty())
        // {
        //     std::lock_guard lock(dbg_mut);
        //     row_printf<NY>("CL = {}\n", CL);
        //     row_printf<NY>("CR = {}\n", CR);
        //     row_printf<NY>("CU = {}\n", CU);
        //     fmt::print("x_0 = {}\n", x_0);
        //     fmt::print("x = {}\n", x);
        //     fmt::print("f = {}\n", minf);
        //     fmt::print("{} constraints\n", constraints.size());
        //     for (const auto& c : constr_data)
        //         fmt::print("{}\n", c.debug_str());
        //     std::exit(1);
        // }
        if (!alwaysRet_)
            return {};
    }
    Row<NY> pred_left;
    Row<NY> pred_right;
    for (size_t i = 0; i < NY; i++)
    {
        pred_left(i) = x[i];
        pred_right(i) = x[i+NY];
    }

    return IcmlTupl{pred_left, pred_right, minf};
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::propagate(const ConstrVec& cs, Attacker<NX>& attacker, 
    size_t feature_id, double feature_value, bool useConstraints) const 
-> std::tuple<FunVec, ConstrDataVec>
{
    FunVec updated_constraints;
    // FunArr updated_constraints_U;
    // FunArr updated_constraints_L;
    // FunArr updated_constraints_R;
    ConstrDataVec constr_data;
    // ConstrDataArr constr_data_U;
    // ConstrDataArr constr_data_L;
    // ConstrDataArr constr_data_R;
    // for (size_t i = 0; i < 2*NY; i++)
    // {
    //     constr_data_U[i] = Constr_data<NY>{SIZE_MAX, INFINITY};
    //     constr_data_L[i] = Constr_data<NY>{SIZE_MAX, INFINITY};
    //     constr_data_R[i] = Constr_data<NY>{SIZE_MAX, INFINITY};
    // }
    // std::array<double,10> lower_bound_L = {INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY};
    // std::array<double,10> upper_bound_L = {-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY};
    // std::array<double,10> lower_bound_R = {INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY};
    // std::array<double,10> upper_bound_R = {-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY};
    // std::array<double,10> lower_bound_U = {INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY,INFINITY};
    // std::array<double,10> upper_bound_U = {-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY,-INFINITY};
    if (useConstraints)
    {
    for (const auto& c : cs)
    {
        auto c_left = c.crosses_left(attacker, feature_id, feature_value);
        auto c_right = c.crosses_right(attacker, feature_id, feature_value);
        if (c_left && c_right)
            updated_constraints.push_back(c.encode_for_optimizer(Direction::U));
        else if (c_left)
            updated_constraints.push_back(c.encode_for_optimizer(Direction::L));
        else if (c_right)
            updated_constraints.push_back(c.encode_for_optimizer(Direction::R));
        constr_data.push_back(Constr_data<NY>{c.get_y(), c.get_bound()});
    }
    // for (const auto& c : cs)
    // {
    //     const size_t y = c.get_y();
    //     const double bound = c.get_bound();
    //     const Ineq ineq = c.get_ineq();
    //     auto c_left = c.propagate_left(attacker, feature_id, feature_value);
    //     auto c_right = c.propagate_right(attacker, feature_id, feature_value);
    //     if (c_left && c_right)
    //     {
    //         if (ineq == Ineq::LT && bound < lower_bound_U[y])
    //         {
    //             lower_bound_U[y] = bound;
    //             updated_constraints_U[y] = c.encode_for_optimizer(Direction::U);
    //             constr_data_U[y] = Constr_data<NY>{y, bound};
    //         }
    //         else if (ineq == Ineq::GTE && bound > upper_bound_U[y])
    //         {
    //             upper_bound_U[y] = bound;
    //             updated_constraints_U[y+NY] = c.encode_for_optimizer(Direction::U);
    //             constr_data_U[y+NY] = Constr_data<NY>{y, bound};
    //         }
    //     }
    //     else if (c_left)
    //     {
    //         if (ineq == Ineq::LT && bound < lower_bound_L[y])
    //         {
    //             lower_bound_L[y] = bound;
    //             updated_constraints_L[y] = c.encode_for_optimizer(Direction::L);
    //             constr_data_L[y] = Constr_data<NY>{y, bound};
    //         }
    //         else if (ineq == Ineq::GTE && bound > upper_bound_L[y])
    //         {
    //             upper_bound_L[y] = bound;
    //             updated_constraints_L[y+NY] = c.encode_for_optimizer(Direction::L);
    //             constr_data_L[y+NY] = Constr_data<NY>{y, bound};
    //         }
    //     }
    //     else if (c_right)
    //     {
    //         if (ineq == Ineq::LT && bound < lower_bound_R[y])
    //         {
    //             lower_bound_R[y] = bound;
    //             updated_constraints_R[y] = c.encode_for_optimizer(Direction::R);
    //             constr_data_R[y] = Constr_data<NY>{y, bound};
    //         }
    //         else if (ineq == Ineq::GTE && bound > upper_bound_R[y])
    //         {
    //             upper_bound_R[y] = bound;
    //             updated_constraints_R[y+NY] = c.encode_for_optimizer(Direction::R);
    //             constr_data_R[y+NY] = Constr_data<NY>{y, bound};
    //         }
    //     }
    // }
    // for (size_t i = 0; i < NY; i++)
    // {
    //     if (constr_data_U[i].y != SIZE_MAX)
    //     {
    //         updated_constraints.push_back(updated_constraints_U[i]);
    //         constr_data.push_back(constr_data_U[i]);
    //     }
    //     if (constr_data_U[i+NY].y != SIZE_MAX)
    //     {
    //         updated_constraints.push_back(updated_constraints_U[i+NY]);
    //         constr_data.push_back(constr_data_U[i+NY]);
    //     }
    //     if (constr_data_L[i].y != SIZE_MAX)
    //     {
    //         updated_constraints.push_back(updated_constraints_L[i]);
    //         constr_data.push_back(constr_data_L[i]);
    //     } 
    //     if (constr_data_L[i+NY].y != SIZE_MAX)
    //     {
    //         updated_constraints.push_back(updated_constraints_L[i+NY]);
    //         constr_data.push_back(constr_data_L[i+NY]);
    //     } 
    //     if (constr_data_R[i].y != SIZE_MAX)
    //     {
    //         updated_constraints.push_back(updated_constraints_R[i]);
    //         constr_data.push_back(constr_data_R[i]);
    //     } 
    //     if (constr_data_R[i+NY].y != SIZE_MAX)
    //     {
    //         updated_constraints.push_back(updated_constraints_R[i+NY]);
    //         constr_data.push_back(constr_data_R[i+NY]);
    //     } 
    // }
    }
    return std::make_tuple(updated_constraints, constr_data);
}

template<size_t N>
std::set<size_t> feature_set()
{
    auto gen = [](){static size_t x = 0; return x++;};
    std::set<size_t> out;
    std::generate_n(std::inserter(out, out.begin()), N, gen);
    return out;
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::optimize_gain(const DF<NX>& X, const DF<NY>& y, const IdxVec& rows,
    const std::set<size_t>& feature_blacklist, Attacker<NX>& attacker, CostMap& costs, 
    ConstrVec& constraints, double current_score, Row<NY> current_prediction_score, 
    bool bootstrap_features, size_t n_sample_features, std::mt19937_64& rd, bool par) const -> OptimTupl
{
    double best_gain = 0.0;
    size_t best_split_feature_id = -1;
    double best_split_feature_value = NAN;
    double next_best_split_feature_value = NAN;
    IdxVec best_split_left_id;
    IdxVec best_split_right_id;
    IdxVec best_split_unknown_id;
    NRow best_pred_left;
    NRow best_pred_right;
    double best_residue;

    thread_pool pool;
    std::mutex gain_mut;

    // create a dictionary containing individual values for each feature_id
    //  (limited to the slice of data located at this node)
    // 1. filter out any blacklisted features from the list of features actually considered
    const static std::set<size_t> all_features = feature_set<NX>();
    std::set<size_t> not_bl;
    std::set_difference(all_features.begin(), all_features.end(),
        feature_blacklist.begin(), feature_blacklist.end(),
        std::inserter(not_bl, not_bl.begin()));
    // 2. randomly sample a subset of n features out of the actual features
    std::set<size_t> actual_features;
    if (bootstrap_features)
    {
        const auto n_sample = std::min(not_bl.size(), n_sample_features);
        std::sample(not_bl.begin(), not_bl.end(), 
            std::inserter(actual_features, actual_features.begin()), 
            n_sample, rd);
    }
    else
        actual_features = not_bl;

    std::map<size_t, std::vector<double>> feature_map;
    for (size_t f_id : actual_features)
    {
        std::vector<double> feats(X.rows());
        for (int64_t rid = 0; rid < X.rows(); rid++)
            feats.push_back(X(rid, f_id));
        std::sort(feats.begin(),feats.end());
        auto ep = std::unique(feats.begin(),feats.end());
        feats.resize(std::distance(feats.begin(), ep));
        feature_map[f_id] = feats;
    }

    if (par)
    {

    for (const auto& [feature_id, feats] : feature_map)
    {
        pool.parallelize_loop(0, feats.size(), 
            [&](const size_t& low, const size_t& high){ // block [low, high)
                if (low >= high)
                    return;
                std::vector<double> my_features;
                for (size_t i = low; i < high; i++)
                    my_features.push_back(feats.at(i));
                IdxVec split_left;
                IdxVec split_right;
                IdxVec split_unknown;
                std::optional<IcmlTupl> optimizer_res;

                double m_best_gain = 0.0;
                size_t m_best_split_feature_id = -1;
                double m_best_split_feature_value = NAN;
                IdxVec m_best_split_left_id;
                IdxVec m_best_split_right_id;
                IdxVec m_best_split_unknown_id;
                NRow m_best_pred_left;
                NRow m_best_pred_right;
                double m_best_residue;
                const bool U_C = useConstraints_;

                const size_t n_mf = my_features.size();
                // Util::log("testing feature {}", feature_id);
                for (size_t feats_idx = 0; feats_idx < n_mf; feats_idx++)
                {
                    double feature_value = my_features[feats_idx];
                    // Util::log("feats: {}", feats);
                    if (algo_ == TrainingAlgo::Icml2019)
                    {
                        auto split_res = split_icml2019(X, y, rows, attacker, costs, feature_id, feature_value);
                        split_left = std::get<0>(split_res);
                        split_right = std::get<1>(split_res);
                        split_unknown = std::get<2>(split_res);
                        optimizer_res = std::get<3>(split_res);
                    }
                    else if (algo_ == TrainingAlgo::Robust)
                    {
                        auto split_res = simulate_split(X, rows, attacker, costs, feature_id, feature_value);
                        split_left = std::get<0>(split_res);
                        split_right = std::get<1>(split_res);
                        split_unknown = std::get<2>(split_res);

                        auto [updated_constraints, constr_data] = propagate(constraints, attacker, feature_id, feature_value, U_C);
                        // if (updated_constraints.size() > 0)
                        //     Util::info("propagated {} constraints.", updated_constraints.size());
                        optimizer_res = optimize_loss_under_attack(y, current_prediction_score,
                            split_left, split_right, split_unknown, updated_constraints, constr_data);
                    }
                    else // Standard decision tree
                    {
                        auto split_res = simple_split(X, y, rows, feature_id, feature_value);
                        split_left = std::get<0>(split_res);
                        split_right = std::get<1>(split_res);
                        optimizer_res = std::get<2>(split_res);
                    }
                    if (optimizer_res)
                    {
                        auto optim_res = optimizer_res.value();
                        auto y_pred_left = std::get<0>(optim_res);
                        auto y_pred_right = std::get<1>(optim_res);
                        auto residue = std::get<2>(optim_res);

                        // std::cout << "pred_left: " << y_pred_left;
                        // std::cout << ", pred_right: " << y_pred_right;
                        // std::cout << ", fun: " << residue << "\n";
                        // std::exit(0);

                        // compute gain
                        //Util::log("current score: {}", current_score);
                        double gain = current_score - residue;
                        //Util::log("current gain: {}, best gain: {}, residue: {}", gain, best_gain, residue);
                        //std::exit(0);

                        // if gain obtained with this split simulation is greater than the best gain so far
                        if (gain > m_best_gain)
                        {
                            m_best_gain = gain;
                            m_best_split_feature_id = feature_id;
                            m_best_split_feature_value = feature_value;
                            // if (low+feats_idx < feats.size() - 1)
                            //     next_best_split_feature_values[my_id] = feats[low+feats_idx+1];
                            // else
                            //     next_best_split_feature_values[my_id] = best_split_feature_values[my_id];
                            
                            m_best_split_left_id = split_left;
                            m_best_split_right_id = split_right;
                            m_best_split_unknown_id = split_unknown;
                            m_best_pred_left = y_pred_left;
                            m_best_pred_right = y_pred_right;
                            m_best_residue = residue;
                        }
                    }
                }
                {
                    std::lock_guard gain_lock(gain_mut);
                    if (m_best_gain > best_gain)
                    {
                        best_gain = m_best_gain;
                        best_split_feature_id = m_best_split_feature_id;
                        best_split_feature_value = m_best_split_feature_value;
                        best_split_left_id = m_best_split_left_id;
                        best_split_right_id = m_best_split_right_id;
                        best_split_unknown_id = m_best_split_unknown_id;
                        best_pred_left = m_best_pred_left;
                        best_pred_right = m_best_pred_right;
                        best_residue = m_best_residue;
                    }
                }
            }
        );
    }
    }
    else
    {
        IdxVec split_left;
        IdxVec split_right;
        IdxVec split_unknown;
        std::optional<IcmlTupl> optimizer_res;
        for (const auto& [feature_id, feats] : feature_map)
        {
            // Util::log("testing feature {}", feature_id);
            for (size_t feats_idx = 0; feats_idx < feats.size(); feats_idx++)
            {
                // Util::log("feats: {}", feats);
                double feature_value = feats[feats_idx];
                if (algo_ == TrainingAlgo::Icml2019)
                {
                    auto split_res = split_icml2019(X, y, rows, attacker, costs, feature_id, feature_value);
                    split_left = std::get<0>(split_res);
                    split_right = std::get<1>(split_res);
                    split_unknown = std::get<2>(split_res);
                    optimizer_res = std::get<3>(split_res);
                }
                else if (algo_ == TrainingAlgo::Robust)
                {
                    auto split_res = simulate_split(X, rows, attacker, costs, feature_id, feature_value);
                    split_left = std::get<0>(split_res);
                    split_right = std::get<1>(split_res);
                    split_unknown = std::get<2>(split_res);
                    auto [updated_constraints, constr_data] = propagate(constraints, attacker, feature_id, feature_value, useConstraints_);

                    optimizer_res = optimize_loss_under_attack(y, current_prediction_score,
                        split_left, split_right, split_unknown, updated_constraints, constr_data);
                }
                else // Standard decision tree
                {
                    auto split_res = simple_split(X, y, rows, feature_id, feature_value);
                    split_left = std::get<0>(split_res);
                    split_right = std::get<1>(split_res);
                    optimizer_res = std::get<2>(split_res);
                }
                if (optimizer_res)
                {
                    auto optim_res = optimizer_res.value();
                    auto y_pred_left = std::get<0>(optim_res);
                    auto y_pred_right = std::get<1>(optim_res);
                    auto residue = std::get<2>(optim_res);

                    // std::cout << "pred_left: " << y_pred_left;
                    // std::cout << ", pred_right: " << y_pred_right;
                    // std::cout << ", fun: " << residue << "\n";
                    // std::exit(0);

                    // compute gain
                    //Util::log("current score: {}", current_score);
                    double gain = current_score - residue;
                    //Util::log("current gain: {}, best gain: {}, residue: {}", gain, best_gain, residue);
                    //std::exit(0);

                    // if gain obtained with this split simulation is greater than the best gain so far
                    if (gain > best_gain)
                    {
                        best_gain = gain;
                        best_split_feature_id = feature_id;
                        best_split_feature_value = feature_value;
                        if (feats_idx < feats.size() - 1)
                            next_best_split_feature_value = feats[feats_idx+1];
                        else
                            next_best_split_feature_value = best_split_feature_value;
                        
                        best_split_left_id = split_left;
                        best_split_right_id = split_right;
                        best_split_unknown_id = split_unknown;
                        best_pred_left = y_pred_left;
                        best_pred_right = y_pred_right;
                        best_residue = residue;
                    }
                }
            }
        }
    }
    CostMap costs_left;
    CostMap costs_right;
    ConstrVec constraints_left;
    ConstrVec constraints_right;
    // Continue iff there's an actual gain
    if (best_gain > 0.0)
    {
        const auto NU = best_split_unknown_id.size();
        if (best_split_unknown_id.size() > 0 && algo_ == TrainingAlgo::Icml2019)
        {
            // Assign unknown instance either to left or right split, according to ICML2019 strategy
            for (auto u : best_split_unknown_id)
            {
                if (X(u,best_split_feature_id) <= best_split_feature_value)
                    best_split_left_id.push_back(u);
                else
                    best_split_right_id.push_back(u);
            }
        }
        if (algo_ == TrainingAlgo::Robust && useConstraints_)
        {
            for (const auto& c : constraints)
            {
                auto c_l = c.propagate_left(attacker, best_split_feature_id, best_split_feature_value);
                if (c_l)
                    constraints_left.push_back(c_l.value());
                auto c_r = c.propagate_right(attacker, best_split_feature_id, best_split_feature_value);
                if (c_r)
                    constraints_right.push_back(c_r.value());
            }
        }
        if (algo_ == TrainingAlgo::Robust && NU > 0)
        {
        pool.parallelize_loop(0, NU, 
            [&](const size_t& low, const size_t& high){ // block [low, high)
                if (low >= high)
                    return;
                std::vector<size_t> my_indices;
                ConstrVec my_constraints_left;
                ConstrVec my_constraints_right;
                IdxVec my_left_id;
                IdxVec my_right_id;
                CostMap my_costs;
                const bool U_C = useConstraints_;
                for (size_t i = low; i < high; i++)
                    my_indices.push_back(best_split_unknown_id.at(i));
                // Assign unknown instance either to left or right split, according to the worst-case scenario

                // get the unknown y-values
                const auto y_true_unknown = DF_index<NY>(y, my_indices);
                const auto loss_left = instance_logloss(y_true_unknown, best_pred_left);
                const auto loss_right = instance_logloss(y_true_unknown, best_pred_right);
                // Util::log("unknown_to_left: ({}x{})", unknown_to_left.rows(), unknown_to_left.cols());
                // Util::log("unknown_to_right: ({}x{})", unknown_to_right.rows(), unknown_to_right.cols());
                for (size_t i = 0; i < my_indices.size(); i++)
                {
                    //using namespace std::ranges;
                    const auto u = my_indices[i];
                    auto attacks = attacker.attack(X.row(u), best_split_feature_id, costs.at(u));
                    std::vector<int> min_vec;
                    auto rang1 = std::views::all(attacks) 
                        | std::views::filter([=](const auto& pair){
                            return std::get<0>(pair)(best_split_feature_id) <= best_split_feature_value;})
                        | std::views::transform([](const auto& pair){return std::get<1>(pair);});
                    std::ranges::copy(rang1, std::back_inserter(min_vec));
                    int min_left = *(std::ranges::min_element(min_vec));
                    // Util::log("min_left_vec: {}", min_vec);
                    min_vec.clear();
                    auto rang2 = std::views::all(attacks) 
                        | std::views::filter([=](const auto& pair){
                            return std::get<0>(pair)(best_split_feature_id) > best_split_feature_value;})
                        | std::views::transform([](const auto& pair){return std::get<1>(pair);});
                    std::ranges::copy(rang2, std::back_inserter(min_vec));
                    int min_right = *(std::ranges::min_element(min_vec));
                    // Util::log("min_right_vec: {}", min_vec);
                    // Util::log("min_left: {}, min_right: {}", min_left, min_right);
                    Eigen::Index u_classid;
                    y.row(u).maxCoeff(&u_classid);
                    const size_t u_c = static_cast<size_t>(u_classid);
                    if (loss_left(i) > loss_right(i) 
                        || (loss_left(i) == loss_right(i) && X(u, best_split_feature_id) <= best_split_feature_value))
                    {
                        // Assign unknown instance to left as the loss is larger
                        const double bound = loss_right(i);
                        my_left_id.push_back(u);
                        my_costs[u] = min_left;
                        if (U_C)
                        {
                            my_constraints_left.push_back(Constraint<NX,NY>(X.row(u), u_c, Ineq::GTE, min_left, bound));
                            my_constraints_right.push_back(Constraint<NX,NY>(X.row(u), u_c, Ineq::LT, min_left, bound));
                        }
                    }
                    else if (loss_right(i) > loss_left(i)
                        || (loss_left(i) == loss_right(i) && X(u, best_split_feature_id) > best_split_feature_value))
                    {
                        // Assign unknown instance to right as the loss is larger
                        const double bound = loss_left(i);
                        my_right_id.push_back(u);
                        my_costs[u] = min_right;
                        if (U_C)
                        {
                            my_constraints_left.push_back(Constraint<NX,NY>(X.row(u), u_c, Ineq::LT, min_right, bound));
                            my_constraints_right.push_back(Constraint<NX,NY>(X.row(u), u_c, Ineq::GTE, min_right, bound));
                        }
                    }
                }
                {
                std::lock_guard lock(gain_mut);
                for (auto id : my_left_id)
                    best_split_left_id.push_back(id);
                for (auto id : my_right_id)
                    best_split_right_id.push_back(id);
                for (auto c : my_constraints_left)
                    constraints_left.push_back(c);
                for (auto c : my_constraints_right)
                    constraints_right.push_back(c);
                for (auto [u, c] : my_costs)
                    costs[u] = c;
                }
            }
        );
        }
        for (auto key : best_split_left_id)
            costs_left[key] = costs.at(key);
        for (auto key : best_split_right_id)
            costs_right[key] = costs.at(key);
    }
    // if (!constraints_left.empty() || !constraints_right.empty())
    // {
    //     for (auto& c: constraints_left)
    //         std::cout << c.debug_str();
    //     for (auto& c: constraints_right)
    //         std::cout << c.debug_str();
    // }
    return OptimTupl{best_gain, best_split_left_id, best_split_right_id, best_split_feature_id,
            best_split_feature_value, next_best_split_feature_value, best_pred_left,
            best_pred_right, best_residue, costs_left, costs_right, constraints_left, constraints_right};
}
