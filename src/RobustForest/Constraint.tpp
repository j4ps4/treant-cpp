#include <algorithm>

template<size_t NX, size_t NY>
std::optional<Constraint<NX,NY>> Constraint<NX,NY>::propagate_left(Attacker<NX>& attacker, 
    size_t feature_id, double feature_value) const
{
    // retrieve all the attacks
    auto attacks = attacker.attack(x_, feature_id, cost_);
    // retain only those attacks whose feature value is less than or equal to feature value
    auto it = std::remove_if(attacks.begin(), attacks.end(), [&](auto& atk){
        return std::get<0>(atk)[feature_id] > feature_value;
    });
    attacks.resize(std::distance(attacks.begin(), it));
    if (attacks.empty())
        return {};
    else
    {
        auto min_cost_it = std::min_element(
            attacks.begin(), attacks.end(), [](const auto& el1, const auto& el2){
                return std::get<1>(el1) < std::get<1>(el2);
            });
        return Constraint<NX,NY>(x_,y_,ineq_,std::get<1>(*min_cost_it),bound_);
    }
}

template<size_t NX, size_t NY>
std::optional<Constraint<NX,NY>> Constraint<NX,NY>::propagate_right(Attacker<NX>& attacker, 
    size_t feature_id, double feature_value) const
{
    // retrieve all the attacks
    auto attacks = attacker.attack(x_, feature_id, cost_);
    // retain only those attacks whose feature value is greater than feature value
    auto it = std::remove_if(attacks.begin(), attacks.end(), [&](auto& atk){
        return std::get<0>(atk)[feature_id] <= feature_value;
    });
    attacks.resize(std::distance(attacks.begin(), it));
    if (attacks.empty())
        return {};
    else
    {
        auto min_cost_it = std::min_element(
            attacks.begin(), attacks.end(), [](const auto& el1, const auto& el2){
                return std::get<1>(el1) < std::get<1>(el2);
            });
        return Constraint<NX,NY>(x_,y_,ineq_,std::get<1>(*min_cost_it),bound_);
    }
}

template<size_t NX, size_t NY>
std::function<double(const Row<NY2C>&)> Constraint<NX,NY>::encode_for_optimizer(Direction dir) const
{
    if (dir == Direction::L)
    {
        if (ineq_ == Ineq::LT)
        {
            return [&](const Row<NY2>& pred)->double{
                return -(y_ * (pred.template head<NY>().log())).sum() + (y_ * bound_.log()).sum();
            };
        }
        else
        {
            return [&](const Row<NY2>& pred)->double{
                return (y_ * (pred.template head<NY>().log())).sum() - (y_ * bound_.log()).sum();
            };
        }
    }
    else if (dir == Direction::R)
    {
        if (ineq_ == Ineq::LT)
        {
            return [&](const Row<NY2>& pred)->double{
                return -(y_ * (pred.template tail<NY>().log())).sum() + (y_ * bound_.log()).sum();
            };
        }
        else
        {
            return [&](const Row<NY2>& pred)->double{
                return (y_ * (pred.template tail<NY>().log())).sum() - (y_ * bound_.log()).sum();
            };
        }
    }
    else
    {
        if (ineq_ == Ineq::LT)
        {
            return [&](const Row<NY2>& pred)->double{
                const double s1 = (y_ * (pred.template head<NY>().log())).sum();
                const double s2 = (y_ * (pred.template tail<NY>().log())).sum();
                return -std::max(s1, s2) + (y_ * bound_.log()).sum();
            };
        }
        else
        {
            return [&](const Row<NY2>& pred)->double{
                const double s1 = (y_ * (pred.template head<NY>().log())).sum();
                const double s2 = (y_ * (pred.template tail<NY>().log())).sum();
                return std::min(s1, s2) - (y_ * bound_.log()).sum();
            };
        }
    }
}