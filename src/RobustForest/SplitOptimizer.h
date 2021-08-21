#pragma once

#include <eigen3/Eigen/Dense>
#include <vector>
#include <limits>
#include <array>
#include <tuple>
#include <optional>
#include <map>
#include <set>
#include <nlopt.hpp>

#include "../DF2/DF_util.h"
#include "../Attacker.h"
#include "Constraint.h"
#include "../def.h"

enum class SplitFunction
{
    LogLoss,
    SSE
};

enum class TrainingAlgo
{
    Robust,
    Icml2019
};

template<size_t NX, size_t NY>
class SplitOptimizer
{
    //constexpr double eps = std::numeric_limits<double>::epsilon();
    using NRow = Row<NY>;

    using IcmlTupl = std::tuple<NRow,NRow,double>;
    using IdxVec = std::vector<size_t>;
    using CostVec = std::vector<int>;
    using CostMap = std::map<int64_t, int>;
    using OptimTupl = std::tuple<double,IdxVec,IdxVec,size_t,double,double,NRow,NRow,double,CostMap,CostMap>;
    using ConstrVec = std::vector<Constraint<NX,NY>>;
    using FunVec = std::vector<std::function<double(unsigned, const double*, double*, void*)>>;

    #define NY2 SplitOptimizer<NX,NY>::NY2V
    static constexpr size_t NY2V = 2*NY;
public:
    SplitOptimizer(SplitFunction split, TrainingAlgo algo) :
    split_(split), algo_(algo) {}

    double evaluate_split(const DF<NY>& y_true,
                          const NRow& y_pred) const;

    // TODO: needs to return left and right constraints
    OptimTupl optimize_gain(const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, 
        const std::set<size_t>& feature_blacklist, int n_sample_features, 
        Attacker<NX>& attacker, const CostMap& costs, 
        ConstrVec& constraints, double current_score, Row<NY> current_prediction_score);

    
private:
    static double sse(const DF<NY>& y_true,
                   const Row<NY>& y_pred);

    static double sse_under_attack(const DF<NY>& left,
                                       const DF<NY>& right,
                                       const DF<NY>& unknown,
                                       const Row<NY2>& pred);

    static double logloss(const DF<NY>& y_true,
                   const Row<NY>& y_pred);

    static double logloss_under_attack(const DF<NY>& left,
                                       const DF<NY>& right,
                                       const DF<NY>& unknown,
                                       const Row<NY2>& pred);

    double icml_split_loss(const DF<NY>& y,
        const IdxVec& L, const IdxVec& R);

    std::tuple<IdxVec, IdxVec, IdxVec, std::optional<IcmlTupl>> split_icml2019(
        const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, Attacker<NX>& attacker,
        const CostMap& costs, size_t feature_id, double feature_value
    );

    std::optional<IcmlTupl> optimize_sse_under_attack(const DF<NY>& y, const Row<NY>& current_prediction_score,
        const IdxVec& split_left, const IdxVec& split_right, 
        const IdxVec& split_unknown, const FunVec& constraints) const;

    std::tuple<IdxVec, IdxVec, IdxVec> simulate_split(
        const DF<NX>& X, const IdxVec& rows, Attacker<NX>& attacker,
        const CostMap& costs, size_t feature_id, double feature_value
    );
    
    const SplitFunction split_;
    const TrainingAlgo algo_;
    const nlopt::algorithm optim_algo_ = nlopt::LD_SLSQP;

};

#include "SplitOptimizer.tpp"