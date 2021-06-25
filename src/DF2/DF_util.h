#include <eigen3/Eigen/Dense>

#include <iostream>

template<size_t N>
using DF = Eigen::Array<double,-1,N,Eigen::RowMajor>;

template<size_t N>
using Row = Eigen::Array<double,1,N,Eigen::RowMajor>;

// namespace
// {

// template <typename... Ts, size_t... Is>
// std::ostream& print_helper(std::ostream& os,
//                 const std::tuple<Ts...>& tuple,
//                 std::index_sequence<Is...>)
// {
//     ((os << std::get<Is>(tuple) << ','), ...);
//     return os;
// }

// }

// template<typename... Ts>
// std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& tuple)
// {
//     using Indices = std::index_sequence_for<Ts...>;
//     print_helper(os, tuple, Indices{});
//     return os;
// }
