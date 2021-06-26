#include <eigen3/Eigen/Dense>
#include <vector>
#include <limits>
#include <array>
#include <tuple>
#include <optional>
#include <map>

#include "../DF2/DF_util.h"
#include "../Attacker.h"

enum class SplitFunction
{
    LogLoss,
    SSE
};

template<std::size_t N>
class SplitOptimizer
{
    constexpr double eps = std::numeric_limits<double>::epsilon();
    using NRow = Row<N>;

    using IcmlTupl = std::tuple<NRow,NRow,double>;
    using IdxVec = std::vector<size_t>;
    using CostVec = std::vector<int>;
    using CostMap = std::map<size_t, int>;
    using OptimTupl = std::tuple<double,IdxVec,IdxVec,size_t,double,double,NRow,NRow,double,CostMap,CostMap>;
public:
    SplitOptimizer(SplitFunction split, bool icml2019) :
    split_(split), icml2019_(icml2019) {}

    double evaluate_split(const DF<N>& y_true,
                          const NRow& y_pred) const;

    template<size_t NX>
    OptimTupl optimize_gain(const DF<NX>& X, const DF<N>& y, const IdxVec& rows, int n_sample_features, 
        Attacker<NA>& attacker, const CostVec& costs, double current_score);
    
private:
    static double sse(const Eigen::ArrayXXd& y_true,
                   const Arr& y_pred);

    static double logloss(const Eigen::ArrayXXd& y_true,
                   const Arr& y_pred);

    static double logloss_under_attack(const Eigen::ArrayXXd& left,
                                       const Eigen::ArrayXXd& right,
                                       const Eigen::ArrayXXd& unknown,
                                       const Arr& pred);

    static double icml_split_loss(const Eigen::ArrayXXd& y,
        const IdxVec& L, const IdxVec& R);

    template<size_t NA>
    static std::tuple<IdxVec, IdxVec, IdxVec, std::optional<IcmlTupl>> split_icml2019(
        const Eigen::ArrayXXd& X, const Eigen::ArrayXXd& y, const IdxVec& rows, Attacker<NA>& attacker,
        std::vector<int>& costs, size_t feature_id, double feature_value
    );

    template<size_t NA>
    static std::tuple<IdxVec, IdxVec, IdxVec> simulate_split(
        const Eigen::ArrayXXd& X, const IdxVec& rows, Attacker<NA>& attacker,
        std::vector<int>& costs, size_t feature_id, double feature_value
    );
    
    SplitFunction split_;
    bool icml2019_;

};

#include "SplitOptimizer.tpp"