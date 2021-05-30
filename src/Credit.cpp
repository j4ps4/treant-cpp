#include <filesystem>

#include "Credit.h"
#include "AttackerRule.h"
#include "DF2/DF_read.h"

#define ATTACK_TYPES int8_t,double,int8_t,double
//#define ATTACK_TYPES double,double,double,double

using FEATURE_ID = std::index_sequence<5,11,6,0>;

namespace credit
{

const std::filesystem::path data_dir = "data/credit/";
const std::filesystem::path data_file = data_dir / "valid.csv.bz2";
const std::filesystem::path json_file = data_dir / "attacks.json";
const std::filesystem::path attack_file = data_dir / "credit.cereal";

cpp::result<DF<DATATYPES>,std::string> read_bz2()
{
    return df::read_bz2<DATATYPES>(data_file.c_str());
}

cpp::result<Attacker<DATATYPES>,std::string> new_Attacker(int budget)
{
    auto res = load_attack_rules<ATTACK_TYPES>(json_file);
    if (res.has_error())
        return cpp::failure(res.error());
    auto& rulz = res.value();
    return Attacker<DATATYPES>(std::move(rulz), budget);
}

void compute_or_load_attacks(Attacker<DATATYPES>& atkr, const DF<DATATYPES>& X)
{
    if (std::filesystem::exists(attack_file))
    {
        Util::info("loading attacked dataset from {}...", attack_file.c_str());
        atkr.load_attacks(attack_file);
    }
    else
    {
        Util::info("computing attacks and storing into {}...", attack_file.c_str());
        atkr.compute_attacks<ATTACK_TYPES>(X, attack_file, FEATURE_ID{});
    }
}

void dump_attack_rules(const Attacker<DATATYPES>& atkr)
{
    atkr.print_rules<ATTACK_TYPES>();
}

}