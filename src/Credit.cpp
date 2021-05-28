#include "Credit.h"
#include "AttackerRule.h"
#include "DF2/DF_read.h"

namespace credit
{
cpp::result<DF<DATATYPES>,std::string> read_bz2(const char* fn)
{
    return df::read_bz2<DATATYPES>(fn);
}

cpp::result<Attacker<DATATYPES>,std::string> new_Attacker(const char* atk_fn, int budget)
{
    auto res = load_attack_rules("attacks.json");
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    return Attacker<DATATYPES>(rulz, budget);
}

}