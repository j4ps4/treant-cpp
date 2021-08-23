#include <limits>
#include <algorithm>
#include <ranges>

#include "../DF2/DF_util.h"

template<size_t NY>
Row<NY> num_classes(const DF<NY>& y)
{
	return y.colwise().sum();
}

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

unsigned count__ = 0;

template<size_t NY>
static double logloss_nlopt(unsigned n, const double* x, double* grad, void* data)
{
    loss_data<NY>* d = static_cast<loss_data<NY>*>(data);
    // const auto* left = d->L;
    // const auto* unknown = d->U;
    // const auto* right = d->R;
    const auto& cL = d->C_L;
    const auto& cU = d->C_U;
    const auto& cR = d->C_R;

    count__++;

    // safe log
    auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};

    // unknowns to the left branch
    if (x[0] <= x[2] && x[1] <= x[3])
    {
        if (grad != nullptr)
        {
            grad[0] = (cL(0)+cU(0)) * -1.0/x[0] + (cL(0)+cL(1)+cU(0)+cU(1));
            grad[1] = (cL(1)+cU(1)) * -1.0/x[1] + (cL(0)+cL(1)+cU(0)+cU(1));
            grad[2] = cR(0) * -1.0/x[2] + (cR(0)+cR(1));
            grad[3] = cR(1) * -1.0/x[3] + (cR(0)+cR(1));
        }
        return (cL(0)+cU(0)) * -mlog(x[0]) + (cL(1)+cU(1)) * -mlog(x[1])
            + cR(0) * -mlog(x[2]) + cR(1) * -mlog(x[3])
            + (cL(0)+cL(1)+cU(0)+cU(1)) * (x[0]+x[1])
            + (cR(0)+cR(1)) * (x[2]+x[3]);
    }
    // 0-unknowns to the left branch
    else if (x[0] <= x[2] && x[1] > x[3])
    {
        if (grad != nullptr)
        {
            grad[0] = (cL(0)+cU(0)) * -1.0/x[0] + (cL(0)+cL(1)+cU(0));
            grad[1] = cL(1) * -1.0/x[1] + (cL(0)+cL(1)+cU(0));
            grad[2] = cR(0) * -1.0/x[2] + (cR(0)+cR(1)+cU(1));
            grad[3] = (cR(1)+cU(1)) * -1.0/x[3] + (cR(0)+cR(1)+cU(1));
        }
        return  (cL(0)+cU(0)) * -mlog(x[0]) + cL(1) * -mlog(x[1])
            + cR(0) * -mlog(x[2]) + (cR(1)+cU(1)) * -mlog(x[3])
            + (cL(0)+cL(1)+cU(0)) * (x[0]+x[1])
            + (cR(0)+cR(1)+cU(1)) * (x[2]+x[3]);
    }
    // 1-unknowns to the left branch
    else if (x[0] > x[2] && x[1] <= x[3])
    {
        if (grad != nullptr)
        {
            grad[0] = cL(0) * -1.0/x[0] + (cL(0)+cL(1)+cU(1));
            grad[1] = (cL(1)+cU(1)) * -1.0/x[1] + (cL(0)+cL(1)+cU(1)); 
            grad[2] = (cR(0)+cU(0)) * -1.0/x[2] + (cR(0)+cR(1)+cU(0));
            grad[3] = cR(1) * -1.0/x[3] + (cR(0)+cR(1)+cU(0));
        }
        return cL(0) * -mlog(x[0]) + (cL(1)+cU(1)) * -mlog(x[1])
            + (cR(0)+cU(0)) * -mlog(x[2]) + cR(1) * -mlog(x[3])
            + (cL(0)+cL(1)+cU(1)) * (x[0]+x[1])
            + (cR(0)+cR(1)+cU(0)) * (x[2]+x[3]);
    }
    // unknowns to the right branch
    else
    {
        if (grad != nullptr)
        {
            grad[0] = cL(0) * -1.0/x[0] + (cL(0)+cL(1));
            grad[1] = cL(1) * -1.0/x[1] + (cL(0)+cL(1));
            grad[2] = (cR(0)+cU(0)) * -1.0/x[2] + (cR(0)+cR(1)+cU(0)+cU(1));
            grad[3] = (cR(1)+cU(1)) * -1.0/x[3] + (cR(0)+cR(1)+cU(0)+cU(1));
        }
        return cL(0) * -mlog(x[0]) + cL(1) * -mlog(x[1])
            + (cR(0)+cU(0)) * -mlog(x[2]) + (cR(1)+cU(1)) * -mlog(x[3])
            + (cL(0)+cL(1)) * (x[0]+x[1])
            + (cR(0)+cR(1)+cU(0)+cU(1)) * (x[2]+x[3]);
    }
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
double SplitOptimizer<NX,NY>::icml_split_loss(const DF<NY>& y,
    const IdxVec& L, const IdxVec& R)
{
    if (L.empty())
    {
        auto R_filt = DF_index<NY>(y, R);
        NRow pred_right = R_filt.colwise().mean();
        double icml_loss = evaluate_split(R_filt, pred_right);
        return icml_loss;
    }
    else if (R.empty())
    {
        auto L_filt = DF_index<NY>(y, L);
        NRow pred_left = L_filt.colwise().mean();
        double icml_loss = evaluate_split(L_filt, pred_left);
        return icml_loss;
    }
    else
    {
        auto L_filt = DF_index<NY>(y, L);
        NRow pred_left = L_filt.colwise().mean();
        auto R_filt = DF_index<NY>(y, R);
        NRow pred_right = R_filt.colwise().mean();
        double icml_loss = evaluate_split(L_filt, pred_left) + evaluate_split(R_filt, pred_right);
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
        auto y_pred_left = DF_index<NY>(y, tmpLeft).colwise().mean();
        auto y_pred_right = DF_index<NY>(y, tmpRight).colwise().mean();
        IdxVec tmp(split_unknown_right.begin(), split_unknown_right.end());
        tmp.insert(tmp.end(), split_unknown_left.begin(), split_unknown_left.end());
        return {split_left, split_right, tmp, std::make_tuple(y_pred_left, y_pred_right, sse)};
    }
}

template<size_t NX, size_t NY>
auto SplitOptimizer<NX,NY>::simulate_split(
    const DF<NX>& X, const IdxVec& rows, Attacker<NX>& attacker,
    const CostMap& costs, size_t feature_id, double feature_value
) -> std::tuple<IdxVec, IdxVec, IdxVec>
{
    IdxVec split_left;
    IdxVec split_right;
    IdxVec split_unknown;

    for (auto row_id : rows)
    {
        int cost = costs.at(row_id);
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

    try
    {
        // set up nlopt
        nlopt::opt optimizer(optim_algo_, NY2);
        optimizer.set_lower_bounds(0);
        optimizer.set_upper_bounds(1);
        double tol = 1e-4;
        optimizer.set_xtol_rel(tol);
        loss_data<NY> d = {CL, CU, CR};
        if (split_ == SplitFunction::LogLoss)
            optimizer.set_min_objective(logloss_nlopt<NY>, &d);
        else
            throw std::runtime_error("not implemented");

        for (size_t i = 0; i < constraints.size(); i++)
        {
            auto fun = constraints[i];
            auto c_data = &constr_data[i];
            optimizer.add_inequality_constraint(*(fun.target<double(*)(unsigned,const double*,double*,void*)>()), c_data, 1e-8);
        }

        double minf;
        auto result = optimizer.optimize(x, minf);
        //Util::info("nlopt_result, after {} iterations: {}", count__, nlopt_result_to_string(static_cast<nlopt_result>(result)));
        count__ = 0;
        // if (result > 0)
        // {

        // }
        // else
        //     Util::die("nlopt error: {}", nlopt_result_to_string(result));
        Row<NY> pred_left;
        Row<NY> pred_right;
        pred_left(0) = x[0];
        pred_left(1) = x[1];
        pred_right(0) = x[2];
        pred_right(1) = x[3];

        return IcmlTupl{pred_left, pred_right, minf};
    }
    catch(std::exception& e)
    {
        Util::die("NLOPT exception: {}", e.what());
    }
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
    ConstrVec constr_left;
    ConstrVec constr_right;
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
            else
            {
                auto split_res = simulate_split(X, rows, attacker, costs, feature_id, feature_value);
                split_left = std::get<0>(split_res);
                split_right = std::get<1>(split_res);
                split_unknown = std::get<2>(split_res);

                FunVec updated_constraints;
                std::vector<Constr_data<NY>> constr_data;
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
                Util::log("current score: {}", current_score);
                double gain = current_score - residue;
                Util::log("current gain: {}, best gain: {}, residue: {}", gain, best_gain, residue);
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
        else
        {
            // Assign unknown instance either to left or right split, according to the worst-case scenario
            auto norm = [](const DF<NY>& df){return df.pow(2).rowwise().sum().sqrt();};

            // get the unknown y-values
            auto y_true_unknown = DF_index<NY>(y, best_split_unknown_id);
            auto unknown_to_left = norm(y_true_unknown - best_pred_left);
            auto unknown_to_right = norm(y_true_unknown - best_pred_right);
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
                min_vec.clear();
                auto rang2 = std::views::all(attacks) 
                    | std::views::filter([=](const auto& pair){
                        return std::get<0>(pair)(best_split_feature_id) > best_split_feature_value;})
                    | std::views::transform([](const auto& pair){return std::get<1>(pair);});
                std::ranges::copy(rang2, std::back_inserter(min_vec));
                int min_right = *(std::ranges::min_element(min_vec));
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
    return OptimTupl{best_gain, best_split_left_id, best_split_right_id, best_split_feature_id,
            best_split_feature_value, next_best_split_feature_value, best_pred_left,
            best_pred_right, best_residue, costs_left, costs_right, constraints_left, constraints_right};
}