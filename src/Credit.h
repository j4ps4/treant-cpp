#pragma once

#include <string_view>
#include <array>
#include <stddef.h>

#define CREDIT 1
constexpr size_t CREDIT_X = 23;
constexpr size_t CREDIT_Y = 2;

#include <string>
#include "result.hpp"
#include "DF2/DF_def.h"
#include "Attacker.h"

namespace credit
{
	// class DataFrame
	// {
	// 	friend class Hostile;
	// public:
	// 	explicit DataFrame(size_t cap);
	// 	DataFrame(DF<CREDIT_DATATYPES>&& df);
	// 	friend DataFrame append(const DataFrame& lhs, const DataFrame& rhs);
	// 	size_t height() const noexcept {return df_.height();}
	// 	constexpr size_t width() const noexcept {return df_.width();}
	// private:
	// 	DF<CREDIT_DATATYPES> df_;
	// };
	
	// enum class ForceCompute { Yes, No };
	
	// class Hostile
	// {
	// public:
	// 	Hostile(Attacker<CREDIT_DATATYPES>&& atkr);
	// 	void attack_dataset(const DataFrame& X, ForceCompute force = ForceCompute::No );
	// 	void dump_attack_rules() const;
	// private:
	// 	std::filesystem::path attack_filename() const;
	// 	Attacker<CREDIT_DATATYPES> atkr_;
	// };
		

    // Read bzipped csv to dataframe.
    cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_bz2();

    // Create attacker from given attacks file and budget.
	cpp::result<Attacker<CREDIT_X>,std::string> new_Attacker(int budget, const DF<CREDIT_X>& X);

}
