#include <algorithm>
#include "../def.h"

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

template<size_t N>
bool check_x(const double* x)
{
    for (size_t i = 0; i < N; i++)
        if (x[i] == 0.0 || x[i] == 1.0)
            return false;
    return true;
}
template<size_t N>
void print_x(const double* x)
{
    for (size_t i = 0; i < N; i++)
        std::cout << x[i] << ", ";
    std::cout << "\n";
}

static auto mlog = [](double x){return std::log(std::min(std::max(x,EPS),1-EPS));};

template<size_t NY, size_t NY2>
double ineq_L_LT(unsigned int n, const double* x, double* grad, void* data)
{
    // if (!check_x<NY2>(x))
    // {
    //     fmt::print("ERROR ineq_L_LT: faulty x in constraint\n");
    //     print_x<NY2>(x);
    //     std::exit(1);
    // }
    auto c_data = static_cast<Constr_data<NY>*>(data);
    const size_t y = (c_data->y);
    const auto& bound = *(c_data->bound);
    if (grad != nullptr)
    {
        for (size_t i = 0; i < NY2; i++)
            grad[i] = 0.0;
        grad[y] = -1.0 / std::max(EPS,x[y]);
    }
    Eigen::Map<const Row<NY2>> pred(x);
    //return -(y * (pred.template head<NY>().log())).sum() + (y * bound.log()).sum();
    // logloss(predLeft) <= logloss(bound)
    // <=> logloss(predLeft) - logloss(bound) <= 0
    // -log(predLeft) + log(bound)
    auto predLeft = pred.template head<NY>();
    return -mlog(predLeft(y)) + mlog(bound(y));
}
template<size_t NY, size_t NY2>
double ineq_L_GTE(unsigned int n, const double* x, double* grad, void* data)
{
    // if (!check_x<NY2>(x))
    // {
    //     fmt::print("ERROR ineq_L_GTE: faulty x in constraint\n");
    //     print_x<NY2>(x);
    //     std::exit(1);
    // }
    auto c_data = static_cast<Constr_data<NY>*>(data);
    const auto y = (c_data->y);
    const auto& bound = *(c_data->bound);
    if (grad != nullptr)
    {
        for (size_t i = 0; i < NY2; i++)
            grad[i] = 0.0;
        grad[y] = 1.0 / std::max(EPS,x[y]);
    }
    Eigen::Map<const Row<NY2>> pred(x);
    auto predLeft = pred.template head<NY>();
    // sum(y * -log(predLeft)) >= sum(y * -log(bound))
    // <=> -sum(y * -log(predLeft)) + sum(y * -log(bound)) <= 0
    // <=> sum(y * log(predLeft)) + sum(y * -log(bound)) <= 0
    return mlog(predLeft(y)) - mlog(bound(y));
}
template<size_t NY, size_t NY2>
double ineq_R_LT(unsigned int n, const double* x, double* grad, void* data)
{
    // if (!check_x<NY2>(x))
    // {
    //     fmt::print("ERROR ineq_R_LT: faulty x in constraint\n");
    //     print_x<NY2>(x);
    //     std::exit(1);
    // }
    auto c_data = static_cast<Constr_data<NY>*>(data);
    const auto y = (c_data->y);
    const auto& bound = *(c_data->bound);
    if (grad != nullptr)
    {
        for (size_t i = 0; i < NY2; i++)
            grad[i] = 0.0;
        grad[y+NY] = -1.0 / std::max(EPS,x[y+NY]);
    }
    Eigen::Map<const Row<NY2>> pred(x);
    auto predRight = pred.template tail<NY>();
    // sum(y * -log(predRight)) <= sum(y * -log(bound))
    // <=> sum(y * -log(predRight)) - sum(y * -log(bound)) <= 0
    // <=> sum(y * -log(predRight)) + sum(y * log(bound)) <= 0
    return -mlog(predRight(y)) + mlog(bound(y));
}
template<size_t NY, size_t NY2>
double ineq_R_GTE(unsigned int n, const double* x, double* grad, void* data)
{
    // if (!check_x<NY2>(x))
    // {
    //     fmt::print("ERROR ineq_R_GTE: faulty x in constraint\n");
    //     print_x<NY2>(x);
    //     std::exit(1);
    // }
    auto c_data = static_cast<Constr_data<NY>*>(data);
    const auto& y = (c_data->y);
    const auto& bound = *(c_data->bound);
    if (grad != nullptr)
    {
        for (size_t i = 0; i < NY2; i++)
            grad[i] = 0.0;
        grad[y+NY] = 1.0 / std::max(EPS,x[y+NY]);
    }
    Eigen::Map<const Row<NY2>> pred(x);
    auto predRight = pred.template tail<NY>();
    return mlog(predRight(y)) - mlog(bound(y));
}
template<size_t NY, size_t NY2>
double ineq_U_LT(unsigned int n, const double* x, double* grad, void* data)
{
    // if (!check_x<NY2>(x))
    // {
    //     fmt::print("ERROR ineq_U_LT: faulty x in constraint\n");
    //     print_x<NY2>(x);
    //     std::exit(1);
    // }
    auto c_data = static_cast<Constr_data<NY>*>(data);
    const auto y = (c_data->y);
    const auto& bound = *(c_data->bound);
    Eigen::Map<const Row<NY2>> pred(x);
    auto predLeft = pred.template head<NY>();
    auto predRight = pred.template tail<NY>();
    const double s1 = -mlog(predLeft(y));
    const double s2 = -mlog(predRight(y));
    if (grad != nullptr)
    {
        for (size_t i = 0; i < NY2; i++)
            grad[i] = 0.0;
        if (s1 >= s2)
        {
            grad[y] = -1.0 / std::max(EPS,x[y]);
        }
        else
        {
            grad[y+NY] = -1.0 / std::max(EPS,x[y+NY]);
        }
    }
    // max(logloss(predLeft), logloss(predRight)) <= logloss(bound)
    // <=> max(..) - logloss(bound) <= 0
    return std::max(s1, s2) + mlog(bound(y));
}
template<size_t NY, size_t NY2>
double ineq_U_GTE(unsigned int n, const double* x, double* grad, void* data)
{
    // if (!check_x<NY2>(x))
    // {
    //     fmt::print("ERROR ineq_U_GTE: faulty x in constraint\n");
    //     print_x<NY2>(x);
    //     std::exit(1);
    // }
    auto c_data = static_cast<Constr_data<NY>*>(data);
    const auto y = (c_data->y);
    const auto& bound = *(c_data->bound);
    Eigen::Map<const Row<NY2>> pred(x);
    auto predLeft = pred.template head<NY>();
    auto predRight = pred.template tail<NY>();
    const double s1 = -mlog(predLeft(y));
    const double s2 = -mlog(predRight(y));
    if (grad != nullptr)
    {
        for (size_t i = 0; i < NY2; i++)
            grad[i] = 0.0;
        if (s1 < s2)
        {
            grad[y] = 1.0 / std::max(EPS,x[y]);
        }
        else
        {
            grad[y+NY] = 1.0 / std::max(EPS,x[y+NY]);
        }
    }
    // min(ll0, ll1) >= logloss(bound)
    // <=> -min(..) + logloss(bound) <= 0
    return -std::min(s1, s2) - mlog(bound(y));
}

template<size_t NX, size_t NY>
std::function<double(unsigned, const double*, double*, void*)> 
Constraint<NX,NY>::encode_for_optimizer(Direction dir) const
{
    if (dir == Direction::L)
    {
        if (ineq_ == Ineq::LT)
        {
            return ineq_L_LT<NY,NY2C>;
        }
        else
        {
            return ineq_L_GTE<NY,NY2C>;
        }
    }
    else if (dir == Direction::R)
    {
        if (ineq_ == Ineq::LT)
        {
            return ineq_R_LT<NY,NY2C>;
        }
        else
        {
            return ineq_R_GTE<NY,NY2C>;
        }
    }
    else
    {
        if (ineq_ == Ineq::LT)
        {
            return ineq_U_LT<NY,NY2C>;
        }
        else
        {
            return ineq_U_GTE<NY,NY2C>;
        }
    }
}

template<size_t NX, size_t NY>
std::string Constraint<NX, NY>::debug_str() const
{
    std::stringstream ss;
    ss << "Constraint:\n\ty: ";
    ss << y_ << "\n\tIneq: ";
    ss << (ineq_ == Ineq::GTE ? ">=" : "<") << "\n\tcost: ";
    ss << cost_ << "\n\tbound: ";
    ss << bound_ << "\n";
    return ss.str();
}

template<size_t NY>
std::string Constr_data<NY>::debug_str() const
{
    std::stringstream ss;
    // if (direct == Direction::L)
    // {
    //     if (ineq == Ineq::LT)
    //     {
    //         ss << "sum(" << *y
    //         return ineq_L_LT<NY,NY2C>;
    //     }
    //     else
    //     {
    //         return ineq_L_GTE<NY,NY2C>;
    //     }
    // }
    // else if (direct == Direction::R)
    // {
    //     if (ineq == Ineq::LT)
    //     {
    //         return ineq_R_LT<NY,NY2C>;
    //     }
    //     else
    //     {
    //         return ineq_R_GTE<NY,NY2C>;
    //     }
    // }
    // else
    // {
    //     if (ineq == Ineq::LT)
    //     {
    //         return ineq_U_LT<NY,NY2C>;
    //     }
    //     else
    //     {
    //         return ineq_U_GTE<NY,NY2C>;
    //     }
    // }
    ss << "y: " << y << "\n";
    ss << "bound: " << *bound << "\n";
    return ss.str();
}