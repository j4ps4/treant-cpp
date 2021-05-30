#pragma once

#include <string_view>
#include <array>

#define DATATYPES double, bool, int8_t, int8_t, uint8_t,\
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
    // Read bzipped csv to dataframe.
    cpp::result<DF<DATATYPES>,std::string> read_bz2(const char* fn);
    // Create attacker from given attacks file and budget.
    cpp::result<Attacker<DATATYPES>,std::string> new_Attacker(const char* atk_fn, int budget);
    void compute_attacks(Attacker<DATATYPES>& atkr, const DF<DATATYPES>& X);

    void dump_attack_rules(const Attacker<DATATYPES>& atkr);
}