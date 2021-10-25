#pragma once

#include <optional>
#include <functional>
#include <vector>

#include "../DF2/DF_def.h"
#include "../Attacker.h"

enum class Ineq
{
    LT, GTE
};

enum class Direction
{
    L, R, U
};

template<size_t NY>
struct Constr_data
{
    size_t y;
    double bound;
    // const Ineq ineq;
    // const Direction direct;
    std::string debug_str() const;
};

template<size_t NX, size_t NY>
class Constraint
{
    #define NY2C Constraint<NX,NY>::NY2CV
public:
    static constexpr size_t NY2CV = 2*NY;
    Constraint(const Row<NX>& x, const size_t y, Ineq ineq, int cost, const double bound):
        x_(x), y_(y), ineq_(ineq), cost_(cost), bound_(bound) {}
    std::optional<Constraint<NX,NY>> propagate_left(Attacker<NX>& attacker, size_t feature_id, double feature_value) const;
    std::optional<Constraint<NX,NY>> propagate_right(Attacker<NX>& attacker, size_t feature_id, double feature_value) const;

    std::function<double(unsigned, const double*, double*, void*)> encode_for_optimizer(Direction dir) const;
    const size_t get_y() const noexcept {return y_;}
    const double get_bound() const noexcept {return bound_;}
    const Ineq get_ineq() const noexcept {return ineq_;}
    std::string debug_str() const;
private:
    Row<NX> x_;
    size_t y_;
    Ineq ineq_;
    int cost_;
    double bound_;
};



#include "Constraint.tpp"