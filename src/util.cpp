#include <fmt/core.h>
#include "util.h"

namespace Util {

DataTypeWBool readDt(const std::string& s)
{
    if (s == "i32")
        return DataTypeWBool::Int;
    else if (s == "u32")
        return DataTypeWBool::UInt;
    else if (s == "f64")
        return DataTypeWBool::Double;
    else if (s == "f32")
        return DataTypeWBool::Float;
    else if (s == "i8")
        return DataTypeWBool::Char;
    else if (s == "u8")
        return DataTypeWBool::UChar;
    else if (s == "bool")
        return DataTypeWBool::Bool;
    else if (s.empty())
        return DataTypeWBool::Double;
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
    }
    return os;
}