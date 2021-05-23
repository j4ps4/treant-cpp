#include <fmt/core.h>
#include "util.h"

namespace Util {

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