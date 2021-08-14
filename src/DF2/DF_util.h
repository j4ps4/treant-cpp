#pragma once
#include "DF_def.h"

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>

#include <iostream>
#include <sstream>

template<size_t N>
DF<N> DF_index(const DF<N>& in, const std::vector<size_t>& idxs)
{
    DF<N> out = Eigen::ArrayXXd::Zero(idxs.size(), N);
    for (size_t i = 0; i < idxs.size(); i++)
	{
		auto id = idxs[i];
        out.row(i) << in.row(id);
	}
    return out;
}

template<size_t N>
void row_printf(const char* msg, const Row<N>& row)
{
    std::cout << msg << " [" << row << "]\n";
}

template<size_t N>
struct fmt::formatter<Row<N>>
{

    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const Row<N>& vec, FormatContext& ctx)
    {
        std::stringstream fmts;
        fmts << "[";
	    fmts << vec;
        fmts << "]";
            
        return format_to(
           ctx.out(),
           "{}",
           fmts.str()
        );
    }
};

template<size_t NX, size_t NY>
std::tuple<DF<NX>,DF<NY>> append3(const std::tuple<DF<NX>,DF<NY>>& fst, 
    const std::tuple<DF<NX>,DF<NY>>& snd, 
    const std::tuple<DF<NX>,DF<NY>>& thrd)
{
    const auto rows1 = std::get<0>(fst).rows();
    const auto rows2 = std::get<0>(snd).rows();
    const auto rows3 = std::get<0>(thrd).rows();
    auto total_rows = rows1 + rows2 + rows3;
    DF<NX> X_out = Eigen::ArrayXXd::Zero(total_rows,NX);
    DF<NY> Y_out = Eigen::ArrayXXd::Zero(total_rows,NY);
    size_t out_idx = 0;
    for (int64_t i = 0; i < rows1; i++)
    {
        X_out.row(out_idx) = std::get<0>(fst).row(i);
        Y_out.row(out_idx++) = std::get<1>(fst).row(i);
    }
    for (int64_t i = 0; i < rows2; i++)
    {
        X_out.row(out_idx) = std::get<0>(snd).row(i);
        Y_out.row(out_idx++) = std::get<1>(snd).row(i);
    }
    for (int64_t i = 0; i < rows3; i++)
    {
        X_out.row(out_idx) = std::get<0>(thrd).row(i);
        Y_out.row(out_idx++) = std::get<1>(thrd).row(i);
    }
    return std::make_tuple(X_out, Y_out);
}

template<size_t NX, size_t NY>
std::tuple<DF<NX>,DF<NY>> append2(const std::tuple<DF<NX>,DF<NY>>& fst, 
    const std::tuple<DF<NX>,DF<NY>>& snd)
{
    const auto rows1 = std::get<0>(fst).rows();
    const auto rows2 = std::get<0>(snd).rows();
    auto total_rows = rows1 + rows2;
    DF<NX> X_out = Eigen::ArrayXXd::Zero(total_rows,NX);
    DF<NY> Y_out = Eigen::ArrayXXd::Zero(total_rows,NY);
    size_t out_idx = 0;
    for (int64_t i = 0; i < rows1; i++)
    {
        X_out.row(out_idx) = std::get<0>(fst).row(i);
        Y_out.row(out_idx++) = std::get<1>(fst).row(i);
    }
    for (int64_t i = 0; i < rows2; i++)
    {
        X_out.row(out_idx) = std::get<0>(snd).row(i);
        Y_out.row(out_idx++) = std::get<1>(snd).row(i);
    }
    return std::make_tuple(X_out, Y_out);
}