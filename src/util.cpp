#include <fmt/core.h>
#include "util.h"

namespace Util {

DataType readDt(const std::string& s)
{
    if (s == "str")
        return DataType::String;
    else if (s == "i32")
        return DataType::Int;
    else if (s == "ui32")
        return DataType::UInt;
    else if (s == "f64")
        return DataType::Double;
    else if (s == "f32")
        return DataType::Float;
    else if (s == "i8")
        return DataType::Short;
    else if (s == "ui8")
        return DataType::UShort;
    else if (s == "bool")
        return DataType::Bool;
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
        case DataType::Int:
        {
            os << "i32";
            break;
        }
        case DataType::UInt:
        {
            os << "ui32";
            break;
        }
        case DataType::Double:
        {
            os << "f64";
            break;
        }
        case DataType::Float:
        {
            os << "f32";
            break;
        }
        case DataType::Short:
        {
            os << "i8";
            break;
        }
        case DataType::UShort:
        {
            os << "ui8";
            break;
        }
        case DataType::Bool:
        {
            os << "bool";
            break;
        }
    }
    return os;
}