#pragma once

#include <vector>
#include <array>
#include <memory>
#include <iterator>
#include <tuple>
#include <string_view>

using DT = double;
using IdxT = size_t;

template<typename... Ts>
struct DF
{
    using TupleVec = std::vector<std::tuple<Ts...>>;
    constexpr static size_t NC = sizeof...(Ts);

    explicit DF(std::vector<std::tuple<Ts...>>&& cols);
    DF(size_t cap);

    auto begin() { return data_.begin(); }
    auto cbegin() const { return data_.cbegin(); }
    auto end() { return data_.end(); }
    auto cend() const { return data_.cend(); }

    // copies data
    DF<Ts...> slice(IdxT start, IdxT end) const;

    std::tuple<Ts...>& operator[](IdxT idx) { return data_[idx]; }
    const std::tuple<Ts...>& operator[](IdxT idx) const { return data_[idx]; }

    auto height() const noexcept {return height_;}

    template<typename T>
    std::vector<T> operator[](const std::string_view v) const;

    void append_row(const std::tuple<Ts...>& row);
    //void append_row(DFR&& row);

private:
    std::vector<std::tuple<Ts...>> data_;
    size_t height_ = 0;
};

#include "DF.tpp"