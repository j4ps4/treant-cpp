#include "ConstexprMap.h"
#include "../util.h"

template<typename... Ts>
DF<Ts...>::DF(std::vector<std::tuple<Ts...>>&& cols)
:
    data_(std::forward<std::vector<std::tuple<Ts...>>>(cols))
{
}

template<typename... Ts>
DF<Ts...>::DF(size_t cap)
{
    data_.reserve(cap);
}

template<typename... Ts>
DF<Ts...> DF<Ts...>::slice(IdxT start, IdxT end) const
{
    TupleVec tv;
    tv.reserve(end-start);
    std::copy_n(data_.cbegin()+start, end-start, std::back_inserter(tv));
    return DF(std::move(tv));
}

template<typename... Ts>
template<typename T>
std::vector<T> DF<Ts...>::at(const std::string_view v) const
{
    const auto col_idx = lookup(v);
    std::vector<T> out;
    out.reserve(height());
    for (const auto& t : data_)
        out.push_back(Util::tuple_index<T>(t, col_idx));
    return out;
}

template<typename... Ts>
DF<Ts...> append(const DF<Ts...>& lhs, const DF<Ts...>& rhs)
{
    DF<Ts...> out(lhs.height() +  rhs.height());
    for (const auto& row : lhs)
        out.data_.push_back(row);
    for (const auto& row : rhs)
        out.data_.push_back(row);
    return out;
}

template<typename... Ts>
DF<Ts...> append3(const DF<Ts...>& df1, const DF<Ts...>& df2, const DF<Ts...>& df3)
{
    DF<Ts...> out(df1.height() + df2.height() + df3.height());
    for (const auto& row : df1)
        out.data_.push_back(row);
    for (const auto& row : df2)
        out.data_.push_back(row);
    for (const auto& row : df3)
        out.data_.push_back(row);
    return out;
}

// template<typename... Ts>
// void DF<Ts...>::append_row(const std::tuple<Ts...>& row)
// {

// }

template<typename... Ts>
void DF<Ts...>::append_row(std::tuple<Ts...>&& row)
{
    data_.push_back(std::forward<std::vector<std::tuple<Ts...>>>(row));
}