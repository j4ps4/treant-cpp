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
    Icml2019,
    Standard
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
    using ConstrVec = std::vector<Constraint<NX,NY>>;
    using FunVec = std::vector<std::function<double(unsigned, const double*, double*, void*)>>;
    using OptimTupl = std::tuple<double,IdxVec,IdxVec,size_t,double,double,NRow,NRow,double,CostMap,CostMap,ConstrVec,ConstrVec>;

    #define NY2 SplitOptimizer<NX,NY>::NY2V
    static constexpr size_t NY2V = 2*NY;
public:
    SplitOptimizer(SplitFunction split, TrainingAlgo algo, int maxiter, double epsilon,
        std::set<size_t> perturb_ids, const std::vector<double>& epsilonCoeff = {}) :
        split_(split), algo_(algo), maxiter_(maxiter), epsilon_(epsilon),
        chen_perturb_ids_(perturb_ids), epsilonCoeff_(epsilonCoeff)
    {
        if (algo == TrainingAlgo::Icml2019 && epsilonCoeff.size() < NX)
            throw std::runtime_error("epsilonCoeff too short");
    }

    double evaluate_split(const DF<NY>& y_true,
                          const NRow& y_pred) const;

    OptimTupl optimize_gain(const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, 
        const std::set<size_t>& feature_blacklist, Attacker<NX>& attacker, CostMap& costs, 
        ConstrVec& constraints, double current_score,Row<NY> current_prediction_score, 
        bool bootstrap_features, size_t n_sample_features, std::mt19937_64& rd, bool par) const;

    TrainingAlgo get_algorithm() const noexcept {return algo_;}

    double get_epsilon() const noexcept {return epsilon_;}
    
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

    double split_loss(const DF<NY>& L, const NRow& pred_left,
        const DF<NY>& R, const NRow& pred_right) const;

    std::tuple<IdxVec, IdxVec, IdxVec, std::optional<IcmlTupl>> split_icml2019(
        const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, Attacker<NX>& attacker,
        const CostMap& costs, size_t feature_id, double feature_value
    ) const;

    std::optional<IcmlTupl> optimize_loss_under_attack(const DF<NY>& y, const Row<NY>& current_prediction_score,
        const IdxVec& split_left, const IdxVec& split_right, 
        const IdxVec& split_unknown, FunVec& constraints, std::vector<Constr_data<NY>>& constr_data) const;

    std::tuple<IdxVec, IdxVec, IdxVec> simulate_split(
        const DF<NX>& X, const IdxVec& rows, Attacker<NX>& attacker,
        const CostMap& costs, size_t feature_id, double feature_value
    ) const;

    std::tuple<IdxVec, IdxVec, std::optional<IcmlTupl>> simple_split(
        const DF<NX>& X, const DF<NY>& y, const IdxVec& rows,
        size_t feature_id, double feature_value
    ) const;
    
    const SplitFunction split_;
    const TrainingAlgo algo_;
    const int maxiter_;
    const nlopt::algorithm optim_algo_ = nlopt::LD_SLSQP;
    const double epsilon_;
	const std::set<size_t> chen_perturb_ids_;
    const std::vector<double> epsilonCoeff_;
};

#include "SplitOptimizer.tpp"