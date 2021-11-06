#include <sstream>
#include <fstream>
#include <algorithm>
#include <variant>
#include <json11.hpp>
#include <cctype>
#include <string>
#include <ranges>

#include "AttackerRule.h"

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
std::istream& operator>>(std::istream& is, std::tuple<T1,T2>& pr)
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

static void inner_loop(AttackType type, const json11::Json::array& ft_attk_list, AttkList& out,
    size_t feature_id, bool fix_constant, double epsilon)
{
    for (const auto& ft_attk : ft_attk_list)
    {
        std::tuple<double,double> pre;
        double post;
        if (type == AttackType::Constant)
        {
            if (fix_constant)
                pre = {-1e12, 1e12};
            else
                pre = {0,0};
            post = std::abs(ft_attk["deform"].number_value());
        }
        else
        {
            auto& pre_str = ft_attk["pre"].string_value();
            std::istringstream iss(pre_str);
            iss >> pre;
            post = ft_attk["post"].number_value();
        }
        post = epsilon * post;
        auto cost = ft_attk["cost"].int_value(); // not used
        auto is_num = ft_attk["is_numerical"].bool_value();
        double pre1 = std::get<0>(pre);
        double pre2 = std::get<1>(pre);
        auto pre_conditions = std::make_tuple(feature_id, pre1, pre2);
        auto post_condition = std::make_pair(feature_id, post);
        out.push_back(AttackerRule(pre_conditions, post_condition, cost, is_num));
        if (type == AttackType::Constant && fix_constant)
        {
            auto post_condition_p = std::make_pair(feature_id, -post);
            out.push_back(AttackerRule(pre_conditions, post_condition_p, cost, is_num));
        }
    }
}

static void load_helper(const std::map<std::string, size_t>& column_map, AttackType type,
    json11::Json::array& attacks, AttkList& out, const std::set<size_t>& id_set, double epsilon)
{
    for (auto& attack : attacks)
    {
        auto& att_obj = attack.object_items();
        auto& feature_name = att_obj.cbegin()->first;
        size_t feature_id;
        if (std::ranges::all_of(feature_name, [](const char& c)->bool{return std::isdigit(c);}))
        {
            feature_id = std::stoul(feature_name);
            if (!id_set.empty() && !id_set.contains(feature_id))
                continue;
            auto& ft_attk_list = att_obj.cbegin()->second.array_items();
            inner_loop(type, ft_attk_list, out, feature_id, false, epsilon);
        }
        else if (feature_name == "ID#")
        {
            if (type == AttackType::Constant)
            {
                if (id_set.empty())
                {
                    feature_id = 0;
                    auto& ft_attk_list = att_obj.cbegin()->second.array_items();
                    inner_loop(type, ft_attk_list, out, feature_id, false, epsilon);
                }
                else
                {
                    for (size_t f_id : id_set)
                    {
                        feature_id = f_id;
                        auto& ft_attk_list = att_obj.cbegin()->second.array_items();
                        inner_loop(type, ft_attk_list, out, feature_id, true, epsilon);
                    }
                }
            }
            else
            {
                for (size_t f_id : id_set)
                {
                    feature_id = f_id;
                    auto& ft_attk_list = att_obj.cbegin()->second.array_items();
                    inner_loop(type, ft_attk_list, out, feature_id, false, epsilon);
                }
            }
        }
        else
        {
            feature_id = column_map.at(feature_name);
            auto& ft_attk_list = att_obj.cbegin()->second.array_items();
            inner_loop(type, ft_attk_list, out, feature_id, false, epsilon);
        }
    }
}

}

cpp::result<LoadType, std::string> 
load_attack_rules(const std::filesystem::path& fn, const std::map<std::string,
    size_t>& column_map, const std::set<size_t>& id_set, double epsilon)
{
    AttkList out;
    std::string err;
    AttackType type;
    auto json_str = slurp(fn);
    auto json = json11::Json::parse(json_str, err);
    if (!err.empty())
        return cpp::failure(fmt::format("while parsing {}: {}", fn.c_str(), err));
    auto& jstype = json["type"];
    if (jstype == json11::Json() || jstype.string_value() == "inf_ball")
        type = AttackType::InfBall;
    else if (jstype.string_value() == "normal")
        return cpp::failure(fmt::format("normal attacks not supported"));
    else if (jstype.string_value() == "constant")
        type = AttackType::Constant;
    else
        return cpp::failure(fmt::format("{}: not valid attack type", jstype.string_value()));
    auto& attacks = json["attacks"];
    if (attacks == json11::Json())
        return cpp::failure(fmt::format("{}: not valid attacks file", fn.c_str()));
    auto attacks_arr = attacks.array_items();
    load_helper(column_map, type, attacks_arr, out, id_set, epsilon);
    if (type == AttackType::Constant && !id_set.empty())
        type = AttackType::InfBall;
    Util::info("loaded {} rules.", out.size());
    return std::make_tuple(out, type);
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
