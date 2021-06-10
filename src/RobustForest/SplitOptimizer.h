#include <eigen3/Eigen/Dense>
#include <vector>
#include <limits>
#include <array>

template<std::size_t N>
class SplitOptimizer
{
    constexpr double eps = std::numeric_limits<double>::epsilon();
    using Arr = Eigen::Array<double,1,N>;
public:
    
private:
    static double logloss(const Eigen::ArrayXXd& y_true,
                   const Arr& y_pred);

    static double logloss_under_attack(const Eigen::ArrayXXd& left,
                                       const Eigen::ArrayXXd& right,
                                       const Eigen::ArrayXXd& unknown,
                                       const Arr& pred);

    static std::array<double,3> icml_split_loss(const Eigen::ArrayXXd& y,
        const std::vector<int>& L, const std::vector<int>& R);

};

#include "SplitOptimizer.tpp"