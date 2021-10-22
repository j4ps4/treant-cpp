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

static void inner_loop(AttackType type, const json11::Json::array& ft_attk_list, AttkList& out, size_t feature_id)
{
    for (const auto& ft_attk : ft_attk_list)
    {
        std::tuple<double,double> pre;
        double post;
        if (type == AttackType::Flat)
        {
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
        auto cost = ft_attk["cost"].int_value();
        auto is_num = ft_attk["is_numerical"].bool_value();
        double pre1 = std::get<0>(pre);
        double pre2 = std::get<1>(pre);
        auto pre_conditions = std::make_tuple(feature_id, pre1, pre2);
        auto post_condition = std::make_pair(feature_id, post);
        out.push_back(AttackerRule(pre_conditions, post_condition, cost, is_num));
    }
}

static void load_helper(const std::map<std::string, size_t>& column_map, AttackType type,
    json11::Json::array& attacks, AttkList& out, const std::set<size_t>& id_set)
{
    for (auto& attack : attacks)
    {
        auto& att_obj = attack.object_items();
        auto& feature_name = att_obj.cbegin()->first;
        size_t feature_id;
        if (std::ranges::all_of(feature_name, [](const char& c)->bool{return std::isdigit(c);}))
        {
            feature_id = std::stoul(feature_name);
            auto& ft_attk_list = att_obj.cbegin()->second.array_items();
            inner_loop(type, ft_attk_list, out, feature_id);
        }
        else if (feature_name == "ID#")
        {
            if (type == AttackType::Flat)
            {
                feature_id = 0;
                auto& ft_attk_list = att_obj.cbegin()->second.array_items();
                inner_loop(type, ft_attk_list, out, feature_id);
            }
            else
            {
                for (size_t f_id : id_set)
                {
                    feature_id = f_id;
                    auto& ft_attk_list = att_obj.cbegin()->second.array_items();
                    inner_loop(type, ft_attk_list, out, feature_id);
                }
            }
        }
        else
        {
            feature_id = column_map.at(feature_name);
            auto& ft_attk_list = att_obj.cbegin()->second.array_items();
            inner_loop(type, ft_attk_list, out, feature_id);
        }
    }
}

}

cpp::result<LoadType, std::string> 
load_attack_rules(const std::filesystem::path& fn, const std::map<std::string,
    size_t>& column_map, const std::set<size_t>& id_set)
{
    AttkList out;
    std::string err;
    AttackType type;
    auto json_str = slurp(fn);
    auto json = json11::Json::parse(json_str, err);
    if (!err.empty())
        return cpp::failure(fmt::format("while parsing {}: {}", fn.c_str(), err));
    auto& jstype = json["type"];
    if (jstype == json11::Json() || jstype.string_value() == "normal")
        type = AttackType::Normal;
    else if (jstype.string_value() == "inf_ball")
        type = AttackType::InfBall;
    else if (jstype.string_value() == "flat")
        type = AttackType::Flat;
    else
        return cpp::failure(fmt::format("{}: not valid attack type", jstype.string_value()));
    auto& attacks = json["attacks"];
    if (attacks == json11::Json())
        return cpp::failure(fmt::format("{}: not valid attacks file", fn.c_str()));
    auto attacks_arr = attacks.array_items();
    load_helper(column_map, type, attacks_arr, out, id_set);
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
