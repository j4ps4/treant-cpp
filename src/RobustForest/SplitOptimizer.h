#pragma once

#include <eigen3/Eigen/Dense>
#include <vector>
#include <limits>
#include <array>
#include <tuple>
#include <optional>
#include <map>
#include <set>

#include "../DF2/DF_util.h"
#include "../Attacker.h"
#include "../def.h"

enum class SplitFunction
{
    LogLoss,
    SSE
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
public:
    SplitOptimizer(SplitFunction split, bool icml2019) :
    split_(split), icml2019_(icml2019) {}

    double evaluate_split(const DF<NY>& y_true,
                          const NRow& y_pred) const;

    OptimTupl optimize_gain(const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, 
        const std::set<size_t>& feature_blacklist, int n_sample_features, 
        Attacker<NX>& attacker, const CostMap& costs, double current_score);
    
private:
    static double sse(const DF<NY>& y_true,
                   const Row<NY>& y_pred);

    static double logloss(const DF<NY>& y_true,
                   const Row<NY>& y_pred);

    static double logloss_under_attack(const DF<NY>& left,
                                       const DF<NY>& right,
                                       const DF<NY>& unknown,
                                       const Row<NY>& pred);

    double icml_split_loss(const DF<NY>& y,
        const IdxVec& L, const IdxVec& R);

    std::tuple<IdxVec, IdxVec, IdxVec, std::optional<IcmlTupl>> split_icml2019(
        const DF<NX>& X, const DF<NY>& y, const IdxVec& rows, Attacker<NX>& attacker,
        const CostMap& costs, size_t feature_id, double feature_value
    );

    std::tuple<IdxVec, IdxVec, IdxVec> simulate_split(
        const DF<NX>& X, const IdxVec& rows, Attacker<NX>& attacker,
        const CostMap& costs, size_t feature_id, double feature_value
    );
    
    SplitFunction split_;
    bool icml2019_;

};

#include "SplitOptimizer.tpp"