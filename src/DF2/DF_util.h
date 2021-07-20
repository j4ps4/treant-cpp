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