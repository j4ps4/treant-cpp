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
#include "../thread_pool.hpp"

enum class SplitFunction
{
    LogLoss,
    SSE
};

using IdxVec = std::vector<size_t>;
using CostVec = std::vector<int>;
using CostMap = std::unordered_map<size_t, int>;

template<size_t NX, size_t NY>
class SplitOptimizer
{
    //constexpr double eps = std::numeric_limits<double>::epsilon();
    using NRow = Row<NY>;

    using IcmlTupl = std::tuple<NRow,NRow,double>;
    using ConstrVec = std::vector<Constraint<NX,NY>>;
    using ConstrArr = std::array<Constraint<NX,NY>, 2*NY>;
    using ConstrDataVec = std::vector<Constr_data<NY>>;
    using ConstrDataArr = std::array<Constr_data<NY>, 2*NY>;
    using FunVec = std::vector<std::function<double(unsigned, const double*, double*, void*)>>;
    using FunArr = std::array<std::function<double(unsigned, const double*, double*, void*)>, 2*NY>;
    using OptimTupl = std::tuple<double,IdxVec,IdxVec,size_t,double,double,NRow,NRow,double,CostMap,CostMap,ConstrVec,ConstrVec>;

    #define NY2 SplitOptimizer<NX,NY>::NY2V
    static constexpr size_t NY2V = 2*NY;
public:
    SplitOptimizer(SplitFunction split, TrainingAlgo algo, int maxiter, double epsilon,
        std::set<size_t> perturb_ids, bool alwaysRet, bool useConstraints, const std::vector<double>& epsilonCoeff = {}) :
        split_(split), algo_(algo), maxiter_(maxiter), epsilon_(epsilon),
        chen_perturb_ids_(perturb_ids), alwaysRet_(alwaysRet), useConstraints_(useConstraints), epsilonCoeff_(epsilonCoeff)
    {
        if (algo == TrainingAlgo::Icml2019 && epsilonCoeff.size() < NX)
            throw std::runtime_error("epsilonCoeff too short");
    }

    SplitOptimizer() = default;

    double evaluate_split(const DF<NY>& y_true,
                          const NRow& y_pred) const;

    SplitOptimizer(const SplitOptimizer<NX,NY>& other) = default;
    SplitOptimizer(SplitOptimizer<NX,NY>&& other) = default;
    SplitOptimizer& operator=(const SplitOptimizer<NX,NY>& other) = default;
    SplitOptimizer& operator=(SplitOptimizer<NX,NY>&& other) = default;

    OptimTupl optimize_gain(const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, 
        const std::set<size_t>& feature_blacklist, Attacker<NX>& attacker, const CostMap& costs, 
        const ConstrVec& constraints, double current_score,Row<NY> current_prediction_score, 
        bool bootstrap_features, size_t n_sample_features, std::mt19937_64& rd,
        bool par, thread_pool& pool) const;

    TrainingAlgo get_algorithm() const noexcept {return algo_;}

    double get_epsilon() const noexcept {return epsilon_;}
    
private:

    void pool_worker(const size_t& low, const size_t& high, const DF<NX>& X, const DF<NY>& y, const IdxVec& rows,
    const std::set<size_t>& feature_blacklist, Attacker<NX>& attacker, const CostMap& costs, 
    const ConstrVec& constraints, double current_score, const Row<NY>& current_prediction_score, size_t feature_id, const std::vector<double>& feats,
    std::mutex& gain_mut, double& best_gain, size_t& best_split_feature_id, double& best_split_feature_value, IdxVec& best_split_left_id,
    IdxVec& best_split_right_id, IdxVec& best_split_unknown_id, NRow& best_pred_left, NRow& best_pred_right, double& best_residue) const;

    std::tuple<FunVec, ConstrDataVec> propagate(const ConstrVec& cs, Attacker<NX>& attacker,
        size_t feature_id, double feature_value, bool useConstraints) const;

    static double sse(const DF<NY>& y_true,
                   const Row<NY>& y_pred);

    static double sse_under_attack(const DF<NY>& left,
                                       const DF<NY>& right,
                                       const DF<NY>& unknown,
                                       const Row<NY2>& pred);

    static double logloss(const DF<NY>& y_true,
                   const Row<NY>& y_pred);
    static Eigen::ArrayXd instance_logloss(const DF<NY>& y_true,
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
    
    SplitFunction split_;
    TrainingAlgo algo_;
    int maxiter_;
    nlopt::algorithm optim_algo_ = nlopt::LD_SLSQP;
    double epsilon_;
	std::set<size_t> chen_perturb_ids_;
    std::vector<double> epsilonCoeff_;
    bool alwaysRet_;
    bool useConstraints_;
};

#include "SplitOptimizer.tpp"