#include <limits>
#include <algorithm>
#include <ranges>

#include "../DF2/DF_util.h"

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
    if (y_true.size() == 0)
        return 0.0;
    
    const auto y_pred_prime = y_pred.max(EPS).min(1-EPS).log();
    double accum = 0.0;
    for (int64_t i = 0; i < y_true.rows(); i++)
	{
		accum -= (y_true.row(i) * y_pred_prime).sum();
	}
    return accum;
}

template<size_t NY>
struct loss_data
{
    const Row<NY> C_L;
    const Row<NY> C_U;
    const Row<NY> C_R;
};

static unsigned count__ = 0;

// loss function for nlopt to minimize
template<size_t NY>
static double logloss_nlopt(unsigned n, const double* x, double* grad, void* data)
{
    throw std::runtime_error("logloss_nlopt not implemented for this N");
    return 0.0;
}

template<>
double logloss_nlopt<2>(unsigned n, const double* x, double* grad, void* data)
{
    loss_data<2>* d = static_cast<loss_data<2>*>(data);
    const auto& cL = d->C_L;
    const auto& cU = d->C_U;
    const auto& cR = d->C_R;

    count__++;

    // safe log
    auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};

    const auto cc_0 = x[0] <= x[2] ? std::make_tuple(cL(0)+cU(0), cR(0)) 
                                   : std::make_tuple(cL(0), cR(0)+cU(0));
    const auto cc_1 = x[1] <= x[3] ? std::make_tuple(cL(1)+cU(1), cR(1)) 
                                   : std::make_tuple(cL(1), cR(1)+cU(1));
    
    if (grad != nullptr)
    {
        grad[0] = std::get<0>(cc_0) * -1.0/x[0];
        grad[1] = std::get<0>(cc_1) * -1.0/x[1];
        grad[2] = std::get<1>(cc_0) * -1.0/x[2];
        grad[3] = std::get<1>(cc_1) * -1.0/x[3];
    }
    return std::get<0>(cc_0) * -mlog(x[0])
         + std::get<0>(cc_1) * -mlog(x[1])
         + std::get<1>(cc_0) * -mlog(x[2])
         + std::get<1>(cc_1) * -mlog(x[3]);

}

template<>
double logloss_nlopt<6>(unsigned n, const double* x, double* grad, void* data)
{
    loss_data<6>* d = static_cast<loss_data<6>*>(data);
    const auto& cL = d->C_L;
    const auto& cU = d->C_U;
    const auto& cR = d->C_R;

    count__++;

    // safe log
    auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};

    const auto cc_0 = x[0] <= x[6] ? std::make_tuple(cL(0)+cU(0), cR(0)) 
                                   : std::make_tuple(cL(0), cR(0)+cU(0));
    const auto cc_1 = x[1] <= x[7] ? std::make_tuple(cL(1)+cU(1), cR(1)) 
                                   : std::make_tuple(cL(1), cR(1)+cU(1));
    const auto cc_2 = x[2] <= x[8] ? std::make_tuple(cL(2)+cU(2), cR(2)) 
                                   : std::make_tuple(cL(2), cR(2)+cU(2));
    const auto cc_3 = x[3] <= x[9] ? std::make_tuple(cL(3)+cU(3), cR(3)) 
                                   : std::make_tuple(cL(3), cR(3)+cU(3));
    const auto cc_4 = x[4] <= x[10] ? std::make_tuple(cL(4)+cU(4), cR(4)) 
                                    : std::make_tuple(cL(4), cR(4)+cU(4));
    const auto cc_5 = x[5] <= x[11] ? std::make_tuple(cL(5)+cU(5), cR(5)) 
                                    : std::make_tuple(cL(5), cR(5)+cU(5));

    if (grad != nullptr)
    {
        grad[0] = std::get<0>(cc_0) * -1.0/x[0];
        grad[1] = std::get<0>(cc_1) * -1.0/x[1];
        grad[2] = std::get<0>(cc_2) * -1.0/x[2];
        grad[3] = std::get<0>(cc_3) * -1.0/x[3];
        grad[4] = std::get<0>(cc_4) * -1.0/x[4];
        grad[5] = std::get<0>(cc_5) * -1.0/x[5];
        grad[6] = std::get<1>(cc_0) * -1.0/x[6];
        grad[7] = std::get<1>(cc_1) * -1.0/x[7];
        grad[8] = std::get<1>(cc_2) * -1.0/x[8];
        grad[9] = std::get<1>(cc_3) * -1.0/x[9];
        grad[10] = std::get<1>(cc_4) * -1.0/x[10];
        grad[11] = std::get<1>(cc_5) * -1.0/x[11];
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

template<size_t NX, size_t NY>
double SplitOptimizer<NX,NY>::logloss_under_attack(const DF<NY>& left,
                                       const DF<NY>& right,
                                       const DF<NY>& unknown,
                                       const Row<NY2>& pred) // length 2xN
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
    // Use logloss_nlopt
    if (split_ == SplitFunction::LogLoss)
        return logloss(y_true, y_pred);
    else
        return sse(y_true, y_pred);
}

template<size_t NX, size_t NY>
double SplitOptimizer<NX,NY>::split_loss(const DF<NY>& L, const NRow& pred_left,
    const DF<NY>& R, const NRow& pred_right)
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
    const CostMap& costs, size_t feature_id, double feature_value) 
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
        int cost = costs.at(row_id); // get the i-th cost spent on the i-th instance so far
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
    // Util::log("split_icml2019: split_left: {}, split_right: {}, split_unknown: {}", 
    //     split_left.size(), split_right.size(), split_unknown_left.size()+split_unknown_right.size());
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
        auto pred_left = L.rows()>0 ? L.colwise().mean() : NRow();
        auto pred_right = R.rows()>0 ? R.colwise().mean() : NRow();
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
        auto pred_left = L.rows()>0 ? L.colwise().mean() : NRow();
        auto pred_right = R.rows()>0 ? R.colwise().mean() : NRow();
        icml_options.push_back(split_loss(L, pred_left, R, pred_right));
    }

    // case 3: all left
    icml_left.insert(icml_left.end(), split_unknown_left.begin(), split_unknown_left.end());
    icml_right.clear();
    icml_right.insert(icml_right.end(), split_right.begin(), split_right.end());
    L = DF_index<NY>(y, icml_left);
    R = DF_index<NY>(y, icml_right);
    {
        auto pred_left = L.rows()>0 ? L.colwise().mean() : NRow();
        auto pred_right = R.rows()>0 ? R.colwise().mean() : NRow();
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
        auto pred_left = L.rows()>0 ? L.colwise().mean() : NRow();
        auto pred_right = R.rows()>0 ? R.colwise().mean() : NRow();
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
        auto y_pred_left = DF_index<NY>(y, tmpLeft).colwise().mean();
        auto y_pred_right = DF_index<NY>(y, tmpRight).colwise().mean();
        IdxVec tmp(split_unknown_right.begin(), split_unknown_right.end());
        tmp.insert(tmp.end(), split_unknown_left.begin(), split_unknown_left.end());
        return {split_left, split_right, tmp, std::make_tuple(y_pred_left, y_pred_right, sse)};
    }
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::simple_split(
    const DF<NX>& X, const DF<NY>& y, const IdxVec& rows,
    size_t feature_id, double feature_value)
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
    auto pred_left = L.rows()>0 ? L.colwise().mean() : NRow();
    auto pred_right = R.rows()>0 ? R.colwise().mean() : NRow();
    auto loss = split_loss(L, pred_left, R, pred_right);
    return {split_left, split_right, std::make_tuple(pred_left, pred_right, loss)};
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::simulate_split(
    const DF<NX>& X, const IdxVec& rows, Attacker<NX>& attacker,
    const CostMap& costs, size_t feature_id, double feature_value)
    -> std::tuple<IdxVec, IdxVec, IdxVec>
{
    IdxVec split_left;
    IdxVec split_right;
    IdxVec split_unknown;

    for (auto row_id : rows)
    {
    	// get the cost spent on the i-th instance so far
        int cost = costs.at(row_id);
        // collect all the attacks the attacker can do on the i-th instance
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
    auto L = DF_index<NY>(y, split_left);
    auto U = DF_index<NY>(y, split_unknown);
    auto R = DF_index<NY>(y, split_right);
    auto CL = num_classes<NY>(L);
    auto CU = num_classes<NY>(U);
    auto CR = num_classes<NY>(R);

    // seed
    std::vector<double> x(NY2);
    for (size_t i = 0; i < NY; i++)
    {
        x[i] = current_prediction_score(i);
        x[i+NY] = current_prediction_score(i);
    }

    double minf;
    try
    {
        // set up nlopt
        nlopt::opt optimizer(optim_algo_, NY2);
        optimizer.set_lower_bounds(0-EPS);
        optimizer.set_upper_bounds(1+EPS);
        double tol = 1e-6;
        optimizer.set_xtol_rel(tol);
        optimizer.set_maxeval(maxiter_);
        loss_data<NY> d = {CL, CU, CR};
        if (split_ == SplitFunction::LogLoss)
            optimizer.set_min_objective(logloss_nlopt<NY>, &d);
        else
            throw std::runtime_error("not implemented");

        // if (constraints.size() > 0)
        //     Util::log("using {} constraints.", constraints.size());
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
        // NLOPT documentation states that it's safe to return final value
    }
    catch(std::exception& e)
    {
        Util::warn("caught NLOPT exception: {}", e.what());
        // Return final value, because it seems to be useful
    }
    count__ = 0;
    Row<NY> pred_left;
    Row<NY> pred_right;
    for (size_t i = 0; i < NY; i++)
    {
        pred_left(i) = x[i];
        pred_right(i) = x[i+NY];
    }

    return IcmlTupl{pred_left, pred_right, minf};
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
    const std::set<size_t>& feature_blacklist, int n_sample_features,
    Attacker<NX>& attacker, CostMap& costs,
    ConstrVec& constraints, double current_score, Row<NY> current_prediction_score) -> OptimTupl
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
    IdxVec split_left;
    IdxVec split_right;
    IdxVec split_unknown;
    std::optional<IcmlTupl> optimizer_res;

    // create a dictionary containing individual values for each feature_id
    //  (limited to the slice of data located at this node)
    const static std::set<size_t> all_features = feature_set<NX>();
    std::set<size_t> not_bl;
    std::set_difference(all_features.begin(), all_features.end(),
        feature_blacklist.begin(), feature_blacklist.end(),
        std::inserter(not_bl, not_bl.begin()));

    std::map<size_t, std::vector<double>> feature_map;
    for (size_t f_id : not_bl)
    {
        std::vector<double> feats(X.rows());
        for (int64_t rid = 0; rid < X.rows(); rid++)
            feats.push_back(X(rid, f_id));
        std::sort(feats.begin(),feats.end());
        auto ep = std::unique(feats.begin(),feats.end());
        feats.resize(std::distance(feats.begin(), ep));
        feature_map[f_id] = feats;
    }

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

                FunVec updated_constraints;
                std::vector<Constr_data<NY>> constr_data;
                // if (constraints.size() > 0)
                //     Util::info("propagating {} constraints.", constraints.size());
                for (const auto& c : constraints)
                {
                    auto c_left = c.propagate_left(attacker, feature_id, feature_value);
                    auto c_right = c.propagate_right(attacker, feature_id, feature_value);
                    if (c_left && c_right)
                        updated_constraints.push_back(c.encode_for_optimizer(Direction::U));
                    else if (c_left)
                        updated_constraints.push_back(c.encode_for_optimizer(Direction::L));
                    else if (c_right)
                        updated_constraints.push_back(c.encode_for_optimizer(Direction::R));
                    constr_data.push_back(Constr_data<NY>{c.get_y_ptr(), c.get_bound_ptr()});
                }
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
    CostMap costs_left;
    CostMap costs_right;
    ConstrVec constraints_left;
    ConstrVec constraints_right;
    // Continue iff there's an actual gain
    if (best_gain > 0.0)
    {
        if (best_split_unknown_id.size() > 0)
            Util::info("best_split_unknown size: {}", best_split_unknown_id.size());
        if (algo_ == TrainingAlgo::Icml2019)
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
        else if (algo_ == TrainingAlgo::Robust)
        {
            // Assign unknown instance either to left or right split, according to the worst-case scenario
            auto norm = [](const DF<NY>& df){return df.pow(2).rowwise().sum().sqrt();};
            auto rowwisediff = [](const DF<NY>& df, const Row<NY>& row){
                DF<NY> out(df.rows(), NY);
                for (int64_t i = 0; i < df.rows(); i++)
                {
                    out.row(i) = df.row(i) - row;
                }
                return out;
            };

            // get the unknown y-values
            auto y_true_unknown = DF_index<NY>(y, best_split_unknown_id);
            auto unknown_to_left = norm(rowwisediff(y_true_unknown, best_pred_left));
            auto unknown_to_right = norm(rowwisediff(y_true_unknown, best_pred_right));
            for (const auto& c : constraints)
            {
                auto c_l = c.propagate_left(attacker, best_split_feature_id, best_split_feature_value);
                if (c_l)
                    constraints_left.push_back(c_l.value());
                auto c_r = c.propagate_right(attacker, best_split_feature_id, best_split_feature_value);
                if (c_r)
                    constraints_right.push_back(c_r.value());
            }
            const auto NU = best_split_unknown_id.size();
            for (size_t i = 0; i < NU; i++)
            {
                //using namespace std::ranges;
                auto u = best_split_unknown_id[i];
                auto attacks = attacker.attack(X.row(u), best_split_feature_id, costs.at(u));
                std::vector<int> min_vec;
                auto rang1 = std::views::all(attacks) 
                    | std::views::filter([=](const auto& pair){
                        return std::get<0>(pair)(best_split_feature_id) <= best_split_feature_value;})
                    | std::views::transform([](const auto& pair){return std::get<1>(pair);});
                std::ranges::copy(rang1, std::back_inserter(min_vec));
                int min_left = *(std::ranges::min_element(min_vec));
                Util::log("min_left_vec: {}", min_vec);
                min_vec.clear();
                auto rang2 = std::views::all(attacks) 
                    | std::views::filter([=](const auto& pair){
                        return std::get<0>(pair)(best_split_feature_id) > best_split_feature_value;})
                    | std::views::transform([](const auto& pair){return std::get<1>(pair);});
                std::ranges::copy(rang2, std::back_inserter(min_vec));
                int min_right = *(std::ranges::min_element(min_vec));
                Util::log("min_right_vec: {}", min_vec);
                Util::log("min_left: {}, min_right: {}", min_left, min_right);
                if (unknown_to_left(i) > unknown_to_right(i))
                {
                    // Assign unknown instance to left as the distance is larger
                    best_split_left_id.push_back(u);
                    costs[u] = min_left;
                    constraints_left.push_back(Constraint<NX,NY>(X.row(u), y.row(u), Ineq::GTE, min_left, best_pred_right));
                    constraints_right.push_back(Constraint<NX,NY>(X.row(u), y.row(u), Ineq::LT, min_left, best_pred_right));
                }
                else
                {
                    // Assign unknown instance to right as the distance is larger
                    best_split_right_id.push_back(u);
                    costs[u] = min_right;
                    constraints_left.push_back(Constraint<NX,NY>(X.row(u), y.row(u), Ineq::LT, min_right, best_pred_left));
                    constraints_right.push_back(Constraint<NX,NY>(X.row(u), y.row(u), Ineq::GTE, min_right, best_pred_left));
                }
            }
        }
        for (auto key : best_split_left_id)
            costs_left[key] = costs.at(key);
        for (auto key : best_split_right_id)
            costs_right[key] = costs.at(key);
    }
    if (!constraints_left.empty() || !constraints_right.empty())
    {
        for (auto& c: constraints_left)
            std::cout << c.debug_str();
        for (auto& c: constraints_right)
            std::cout << c.debug_str();
    }
    return OptimTupl{best_gain, best_split_left_id, best_split_right_id, best_split_feature_id,
            best_split_feature_value, next_best_split_feature_value, best_pred_left,
            best_pred_right, best_residue, costs_left, costs_right, constraints_left, constraints_right};
}
