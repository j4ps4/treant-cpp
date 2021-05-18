#include <fmt/core.h>
#include "util.h"

namespace Util {

DataType readDt(const std::string& s)
{
    if (s == "i32")
        return DataType::Int;
    else if (s == "u32")
        return DataType::UInt;
    else if (s == "f64")
        return DataType::Double;
    else if (s == "f32")
        return DataType::Float;
    else if (s == "i8")
        return DataType::Char;
    else if (s == "u8")
        return DataType::UChar;
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
        case DataType::Int:
        {
            os << "i32";
            break;
        }
        case DataType::UInt:
        {
            os << "u32";
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
        case DataType::Char:
        {
            os << "i8";
            break;
        }
        case DataType::UChar:
        {
            os << "u8";
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