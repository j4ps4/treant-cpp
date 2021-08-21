#pragma once

#include <optional>
#include <functional>

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

template<size_t NX, size_t NY>
class Constraint
{
    #define NY2C Constraint<NX,NY>::NY2CV
public:
    static constexpr size_t NY2CV = 2*NY;
    Constraint(const Row<NX>& x, const Row<NY>& y, Ineq ineq, int cost, const Row<NY>& bound):
        x_(x), y_(y), ineq_(ineq), cost_(cost), bound_(bound) {}
    std::optional<Constraint<NX,NY>> propagate_left(Attacker<NX>& attacker, size_t feature_id, double feature_value) const;
    std::optional<Constraint<NX,NY>> propagate_right(Attacker<NX>& attacker, size_t feature_id, double feature_value) const;

    std::function<double(unsigned, const double*, double*, void*)> encode_for_optimizer(Direction dir) const;
private:
    Row<NX> x_;
    Row<NY> y_;
    Ineq ineq_;
    int cost_;
    Row<NY> bound_;
};

#include "Constraint.tpp"