#include <sstream>
#include <fstream>
#include <algorithm>
#include <variant>
#include <json11.hpp>

#include "DF2/ConstexprMap.h"

namespace
{

#define ATTK_TYPES double,float,uint32_t,int32_t,uint8_t,int8_t,bool

// Read file into string.
inline std::string slurp(const std::string& path)
{
    std::ostringstream buf; 
    std::ifstream input(path.c_str()); 
    buf << input.rdbuf(); 
    return buf.str();
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
    is >> val1;
    is.get(c);
    if (c != ',')
    {
        auto s = fmt::format("parse error: expected ',', got {}", c);
        throw std::invalid_argument(s);
    }
    is >> val2;
    is.get(c);
    if (c != ')')
    {
        auto s = fmt::format("parse error: expected ')', got {}", c);
        throw std::invalid_argument(s);
    }
    pr = {val1, val2};
    return is;
}

template<typename F>
void inner_loop(const json11::Json::array& ft_attk_list, AttkList& out, size_t feature_id)
{
    for (const auto& ft_attk : ft_attk_list)
    {
        auto& pre_str = ft_attk["pre"].string_value();
        std::istringstream iss(pre_str);
        std::pair<F,F> pre;
        iss >> pre;
        F post = static_cast<F>(ft_attk["post"].number_value());
        auto cost = ft_attk["cost"].int_value();
        auto is_num = ft_attk["is_numerical"].bool_value();
        std::variant<ATTK_TYPES> pre1 = std::get<0>(pre);
        std::variant<ATTK_TYPES> pre2 = std::get<1>(pre);
        std::variant<ATTK_TYPES> postv = post;
        auto pre_conditions = std::make_tuple(feature_id, pre1, pre2);
        auto post_condition = std::make_pair(feature_id, postv);
        out.push_back(AttackerRule(pre_conditions, post_condition, cost, is_num));
    }
}

template<typename F>
void load_helper(json11::Json::array::const_iterator& attacks_it, AttkList& out)
{
    auto& attack = *attacks_it;
    auto& att_obj = attack.object_items();
    auto& feature_name = att_obj.cbegin()->first;
    auto feature_id = lookup(std::string_view(feature_name.cbegin(), feature_name.cend()));
    auto& ft_attk_list = att_obj.cbegin()->second.array_items();
    inner_loop<F>(ft_attk_list, out, feature_id);
}

template<typename F1, typename F2, typename... Fs>
void load_helper(json11::Json::array::const_iterator& attacks_it, AttkList& out)
{
    auto& attack = *attacks_it;
    auto& att_obj = attack.object_items();
    auto& feature_name = att_obj.cbegin()->first;
    auto feature_id = lookup(std::string_view(feature_name.cbegin(), feature_name.cend()));
    auto& ft_attk_list = att_obj.cbegin()->second.array_items();
    inner_loop<F1>(ft_attk_list, out, feature_id);
    attacks_it++;
    load_helper<F2,Fs...>(attacks_it, out);
}

}

template<typename... Fs>
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
    auto attacks_it = attacks.array_items().begin();
    load_helper<Fs...>(attacks_it, out);
    return out;
}

template<typename F>
std::string AttackerRule::debug_str() const
{
    std::stringstream ss;
    ss << "feature: " << std::get<0>(pre_conditions_) << "\n";
    ss << "pre: (" << std::get<F>(std::get<1>(pre_conditions_)) << ", " 
        << std::get<F>(std::get<2>(pre_conditions_)) << ")\n";
    ss << "post: " << std::get<F>(std::get<1>(post_condition_)) << "\n";
    ss << "cost: " << cost_ << "\n";
    return ss.str();
}
