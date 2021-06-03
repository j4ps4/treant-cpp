#pragma once

#include <string_view>
#include <array>

#define CREDIT_DATATYPES double, bool, int8_t, int8_t, uint8_t,\
  int8_t,int8_t,int8_t,int8_t,int8_t,int8_t,double,double,double,\
  double,double,double,double,double,double,double,double,double,\
  bool
// #define DATATYPES double, double, double, double, double,\
//     double,double,double,double,double,double,double,double,double,\
//     double,double,double,double,double,double,double,double,double,\
//     double

#define CREDIT 1

#include <string>
#include "result.hpp"
#include "DF2/DF.h"
#include "Attacker.h"

namespace credit
{
	class DataFrame
	{
		friend class Hostile;
	public:
		DataFrame(DF<CREDIT_DATATYPES>&& df);
	private:
		DF<CREDIT_DATATYPES> df_;
	};
	
	enum class ForceCompute { Yes, No };
	
	class Hostile
	{
	public:
		Hostile(Attacker<CREDIT_DATATYPES>&& atkr);
		void attack_dataset(const DataFrame& X, ForceCompute force = ForceCompute::No );
		void dump_attack_rules() const;
	private:
		Attacker<CREDIT_DATATYPES> atkr_;
	};
		

    // Read bzipped csv to dataframe.
    cpp::result<DataFrame,std::string> read_bz2();
    // Create attacker from given attacks file and budget.
    cpp::result<Hostile,std::string> new_Hostile(int budget);

}
