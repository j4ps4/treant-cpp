#include <sstream>
#include <fstream>
#include <algorithm>
#include <json11.hpp>

#include "AttackerRule.h"
#include "util.h"

// Read file into string.
inline std::string slurp(const std::string& path)
{
    std::ostringstream buf; 
    std::ifstream input(path.c_str()); 
    buf << input.rdbuf(); 
    return buf.str();
}

std::istream& operator>>(std::istream& is, std::pair<double,double>& pr)
{
    char c;
    is.get(c);
    if (c != '(')
        throw std::invalid_argument("operator>>: expected '('");
    double val1, val2;
    is >> val1;
    is.get(c);
    if (c != ',')
        throw std::invalid_argument("operator>>: expected ','");
    is >> val2;
    is.get(c);
    if (c != ')')
        throw std::invalid_argument("operator>>: expected ')'");
    pr = {val1, val2};
    return is;
}

cpp::result<AttkList, std::string> 
load_attack_rules(const std::string& fn, const ColMap& colmap)
{
    AttkList out;
    std::string err;
    auto json_str = slurp(fn);
    auto json = json11::Json::parse(json_str, err);
    if (!err.empty())
        return cpp::failure(fmt::format("while parsing {}: {}", fn, err));
    auto& attacks = json["attacks"];
    if (attacks == json11::Json())
        return cpp::failure(fmt::format("{}: not valid attacks file", fn));
    auto& attacks_v = attacks.array_items();
    for (auto& attack : attacks_v)
    {
        auto& att_obj = attack.object_items();
        auto& feature_name = att_obj.cbegin()->first;
        const auto& feature_it = std::find_if(colmap.cbegin(), colmap.cend(),[&](const auto& pair){
            auto& colname = pair.second.first;
            return colname == feature_name;
        });
        auto feature_id = feature_it->first;
        auto& ft_attk_list = att_obj.cbegin()->second.array_items();
        for (const auto& ft_attk : ft_attk_list)
        {
            auto& pre_str = ft_attk["pre"].string_value();
            std::istringstream iss(pre_str);
            std::pair<double,double> pre;
            iss >> pre;
            auto post = ft_attk["post"].number_value();
            auto cost = ft_attk["cost"].int_value();
            auto is_num = ft_attk["is_numerical"].bool_value();
            auto pre_conditions = std::tuple_cat(std::make_tuple(feature_id), pre);
            auto post_condition = std::make_pair(feature_id, post);
            out.push_back(AttackerRule(pre_conditions, post_condition, cost, is_num));
        }
    }
    return out;
}

std::string AttackerRule::debug_str() const
{
    std::stringstream ss;
    ss << "feature: " << std::get<0>(pre_conditions_) << "\n";
    ss << "pre: (" << std::get<1>(pre_conditions_) << ", " 
        << std::get<2>(pre_conditions_) << ")\n";
    ss << "post: " << std::get<1>(post_condition_) << "\n";
    ss << "cost: " << cost_ << "\n";
    return ss.str();
}

bool AttackerRule::is_applicable(const DFRView& row) const
{
    auto feature_id = std::get<0>(pre_conditions_);
    auto left = std::get<1>(pre_conditions_);
    auto right = std::get<2>(pre_conditions_);
    auto feature = row.get_as_f64(feature_id);
    auto res = left <= feature && feature <= right;
    return res;
}

DFR AttackerRule::apply(const DFRView& row) const
{
    auto feature_id = std::get<0>(post_condition_);
    auto attack = std::get<1>(post_condition_);
    auto newrow = row.copy();
    newrow.modify_value(feature_id, attack);
    return newrow;
}

bool AttackerRule::is_applicable(const DFR& row) const
{
    auto feature_id = std::get<0>(pre_conditions_);
    auto left = std::get<1>(pre_conditions_);
    auto right = std::get<2>(pre_conditions_);
    auto feature = row.get_as_f64(feature_id);
    auto res = left <= feature && feature <= right;
    return res;
}

DFR AttackerRule::apply(const DFR& row) const
{
    auto feature_id = std::get<0>(post_condition_);
    auto attack = std::get<1>(post_condition_);
    auto newrow = row;
    newrow.modify_value(feature_id, attack);
    return newrow;
}