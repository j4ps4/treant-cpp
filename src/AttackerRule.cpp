#include <sstream>
#include <fstream>
#include <algorithm>
#include <json11.hpp>

#include "AttackerRule.h"
#include "util.h"
#include "DF2/ConstexprMap.h"

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
load_attack_rules(const std::string& fn)
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
        auto feature_id = lookup(std::string_view(feature_name.cbegin(), feature_name.cend()));
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
