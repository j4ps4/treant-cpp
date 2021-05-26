#include "ConstexprMap.h"

template<typename... Ts>
DF<Ts...>::DF(std::vector<std::tuple<Ts...>>&& cols)
:
    data_(cols)
{
    height_ = data_.size();
}

template<typename... Ts>
DF<Ts...>::DF(size_t cap)
{
    data_.reserve(cap);
    height_ = 0;
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
std::vector<T> DF<Ts...>::operator[](const std::string_view v) const
{
    auto col_idx = lookup(v);
    std::vector<T> out;
    out.reserve(height_);
    for (const auto& t : data_)
        out.push_back(std::get<col_idx>(t));
    return out;
}
