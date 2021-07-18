#include <sstream>
#include <fstream>
#include <algorithm>
#include <variant>
#include <json11.hpp>

#include "AttackerRule.h"
#include "DF2/ConstexprMap.h"

namespace
{

// Read file into string.
inline std::string slurp(const std::filesystem::path& path)
{
    std::ostringstream buf; 
    std::ifstream input(path); 
    buf << input.rdbuf(); 
    return buf.str();
}

template<typename T>
std::istream& get_val(std::istream& is, T& val)
{
    is >> val;
    return is;
}
template<>
std::istream& get_val<int8_t>(std::istream& is, int8_t& val)
{
    int temp;
    is >> temp;
    val = temp;
    return is;
}
template<>
std::istream& get_val<uint8_t>(std::istream& is, uint8_t& val)
{
    unsigned temp;
    is >> temp;
    val = temp;
    return is;
}

template<typename T1, typename T2>
std::istream& operator>>(std::istream& is, std::pair<T1,T2>& pr)
{
    char c;
    is.get(c);
    if (c != '(')
    {
        auto s = fmt::format("parse error: expected '(', got {}", c);
        throw std::invalid_argument(s);
    }
    T1 val1; 
    T2 val2;
    get_val(is, val1);
    is.get(c);
    if (c != ',')
    {
        auto s = fmt::format("parse error: expected ',', got {}", c);
        throw std::invalid_argument(s);
    }
    get_val(is, val2);
    is.get(c);
    if (c != ')')
    {
        auto s = fmt::format("parse error: expected ')', got {}", c);
        throw std::invalid_argument(s);
    }
    pr = {val1, val2};
    return is;
}

void inner_loop(const json11::Json::array& ft_attk_list, AttkList& out, size_t feature_id)
{
    for (const auto& ft_attk : ft_attk_list)
    {
        auto& pre_str = ft_attk["pre"].string_value();
        std::istringstream iss(pre_str);
        std::pair<double,double> pre;
        iss >> pre;
        double post = ft_attk["post"].number_value();
        auto cost = ft_attk["cost"].int_value();
        auto is_num = ft_attk["is_numerical"].bool_value();
        double pre1 = std::get<0>(pre);
        double pre2 = std::get<1>(pre);
        auto pre_conditions = std::make_tuple(feature_id, pre1, pre2);
        auto post_condition = std::make_pair(feature_id, post);
        out.push_back(AttackerRule(pre_conditions, post_condition, cost, is_num));
    }
}

void load_helper(json11::Json::array& attacks, AttkList& out)
{
    for (auto& attack : attacks)
    {
        auto& att_obj = attack.object_items();
        auto& feature_name = att_obj.cbegin()->first;
        auto feature_id = lookup(std::string_view(feature_name.cbegin(), feature_name.cend()));
        auto& ft_attk_list = att_obj.cbegin()->second.array_items();
        inner_loop(ft_attk_list, out, feature_id);
    }
}

}

cpp::result<AttkList, std::string> 
load_attack_rules(const std::filesystem::path& fn)
{
    AttkList out;
    std::string err;
    auto json_str = slurp(fn);
    auto json = json11::Json::parse(json_str, err);
    if (!err.empty())
        return cpp::failure(fmt::format("while parsing {}: {}", fn.c_str(), err));
    auto& attacks = json["attacks"];
    if (attacks == json11::Json())
        return cpp::failure(fmt::format("{}: not valid attacks file", fn.c_str()));
    auto attacks_arr = attacks.array_items();
    load_helper(attacks_arr, out);
    return out;
}

std::string AttackerRule::debug_str() const
{
    using Util::numeral;
    std::stringstream ss;
    ss << "feature: " << std::get<0>(pre_conditions_) << "\n";
    ss << "pre: (" << numeral(std::get<1>(pre_conditions_)) << ", " 
        << numeral(std::get<2>(pre_conditions_)) << ")\n";
    ss << "post: " << numeral(std::get<1>(post_condition_)) << "\n";
    ss << "cost: " << cost_ << "\n";
    return ss.str();
}
