#include "Credit.h"
#include "AttackerRule.h"
#include "DF2/DF_read.h"

#define ATTACK_TYPES int8_t,double,int8_t,double
//#define ATTACK_TYPES double,double,double,double

using FEATURE_ID = std::index_sequence<5,11,6,0>;

namespace credit
{
cpp::result<DF<DATATYPES>,std::string> read_bz2(const char* fn)
{
    return df::read_bz2<DATATYPES>(fn);
}

cpp::result<Attacker<DATATYPES>,std::string> new_Attacker(const char* atk_fn, int budget)
{
    auto res = load_attack_rules<ATTACK_TYPES>("attacks.json");
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    return Attacker<DATATYPES>(std::move(rulz), budget);
}

void compute_attacks(Attacker<DATATYPES>& atkr, const DF<DATATYPES>& X)
{
    atkr.compute_attacks<ATTACK_TYPES>(X, "nvm", FEATURE_ID{});
}

void dump_attack_rules(const Attacker<DATATYPES>& atkr)
{
    atkr.print_attacks<ATTACK_TYPES>();
}

}