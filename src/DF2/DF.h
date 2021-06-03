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
    explicit DF(size_t cap);

    auto begin() const { return data_.begin(); }
    auto cbegin() const { return data_.cbegin(); }
    auto end() const { return data_.end(); }
    auto cend() const { return data_.cend(); }

    // copies data
    DF<Ts...> slice(IdxT start, IdxT end) const;

    std::tuple<Ts...>& operator[](IdxT idx) { return data_[idx]; }
    const std::tuple<Ts...>& operator[](IdxT idx) const { return data_[idx]; }

    auto height() const noexcept {return data_.size();}
    constexpr auto width() const noexcept {return NC;}

    void reserve(size_t n) {data_.reserve(n);}
    void append_row(const std::tuple<Ts...>& row) {data_.push_back(row);}

    template<typename T>
    std::vector<T> at(const std::string_view v) const;

    template<typename... AT>
    friend DF<AT...> append(const DF<AT...>& lhs, const DF<AT...>& rhs);
    template<typename... AT>
    friend DF<AT...> append3(const DF<AT...>& df1, const DF<AT...>& df2, const DF<AT...>& df3);

    //void append_row(const std::tuple<Ts...>& row);
    void append_row(std::tuple<Ts...>&& row);

private:
    std::vector<std::tuple<Ts...>> data_;
};

#include "DF.tpp"