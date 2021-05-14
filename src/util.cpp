#include <fmt/core.h>
#include "util.h"

namespace Util {

DataType readDt(const std::string& s)
{
    if (s == "str")
        return DataType::String;
    else if (s == "f64")
        return DataType::Double;
    else if (s == "int")
        return DataType::Int;
    else if (s.empty())
        return DataType::Double;
    else
        die("invalid datatype specifier: {}", s);
}

}

std::ostream& operator<<(std::ostream& os, DataType t)
{
    switch(t)
    {
        case DataType::String:
        {
            os << "str";
            break;
        }
        case DataType::Double:
        {
            os << "f64";
            break;
        }
        case DataType::Int:
        {
            os << "int";
            break;
        }
    }
    return os;
}