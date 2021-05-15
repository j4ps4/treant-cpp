#include <set>
#include <stdexcept>

#include "def.h"
#include "util.h"

namespace
{

std::set<DataType> get_column_types(const ColMap& colmap)
{
    std::set<DataType> out;
    for (const auto& [idx, pair] : colmap)
    {
        out.insert(pair.second);
    }
    return out;
}

inline bool contains(const std::set<DataType>& dtypes, DataType dt)
{
    return dtypes.count(dt) > 0;
}

}


DF DF::get_data_by_idx(hmdf::Index2D<IdxT> idx)
{
    using Bool = bool;
    using Char = signed char;
    using Double = double;
    using Float = float;
    using Int = int;
    using String = std::string;
    using UChar = unsigned char;
    using UInt = unsigned int;
    switch(nDtypes_)
    {
        case 0:
        {
            return DF({},{});
        }
        case 1:
        {
            switch(colMagic_)
            {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool>(idx);
            return DF(std::move(df), colmap_);
            }
            case 2:
            {
            auto df = df_.get_data_by_idx<Char>(idx);
            return DF(std::move(df), colmap_);
            }
            case 3:
            {
            auto df = df_.get_data_by_idx<Double>(idx);
            return DF(std::move(df), colmap_);
            }
            case 4:
            {
            auto df = df_.get_data_by_idx<Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 5:
            {
            auto df = df_.get_data_by_idx<Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 6:
            {
            auto df = df_.get_data_by_idx<String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 7:
            {
            auto df = df_.get_data_by_idx<UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 8:
            {
            auto df = df_.get_data_by_idx<UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
            }
        }
        case 2:
        {
        switch(colMagic_)
        {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool,Char>(idx);
            return DF(std::move(df), colmap_);
            }
            case 2:
            {
            auto df = df_.get_data_by_idx<Bool,Double>(idx);
            return DF(std::move(df), colmap_);
            }
            case 3:
            {
            auto df = df_.get_data_by_idx<Bool,Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 4:
            {
            auto df = df_.get_data_by_idx<Bool,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 5:
            {
            auto df = df_.get_data_by_idx<Bool,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 6:
            {
            auto df = df_.get_data_by_idx<Bool,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 7:
            {
            auto df = df_.get_data_by_idx<Bool,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 8:
            {
            auto df = df_.get_data_by_idx<Char,Double>(idx);
            return DF(std::move(df), colmap_);
            }
            case 9:
            {
            auto df = df_.get_data_by_idx<Char,Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 10:
            {
            auto df = df_.get_data_by_idx<Char,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 11:
            {
            auto df = df_.get_data_by_idx<Char,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 12:
            {
            auto df = df_.get_data_by_idx<Char,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 13:
            {
            auto df = df_.get_data_by_idx<Char,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 14:
            {
            auto df = df_.get_data_by_idx<Double,Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 15:
            {
            auto df = df_.get_data_by_idx<Double,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 16:
            {
            auto df = df_.get_data_by_idx<Double,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 17:
            {
            auto df = df_.get_data_by_idx<Double,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 18:
            {
            auto df = df_.get_data_by_idx<Double,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 19:
            {
            auto df = df_.get_data_by_idx<Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 20:
            {
            auto df = df_.get_data_by_idx<Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 21:
            {
            auto df = df_.get_data_by_idx<Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 22:
            {
            auto df = df_.get_data_by_idx<Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 23:
            {
            auto df = df_.get_data_by_idx<Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 24:
            {
            auto df = df_.get_data_by_idx<Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 25:
            {
            auto df = df_.get_data_by_idx<Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 26:
            {
            auto df = df_.get_data_by_idx<String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 27:
            {
            auto df = df_.get_data_by_idx<String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 28:
            {
            auto df = df_.get_data_by_idx<UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
        }
        }
        case 3:
        {
        switch(colMagic_)
        {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double>(idx);
            return DF(std::move(df), colmap_);
            }
            case 2:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 3:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 4:
            {
            auto df = df_.get_data_by_idx<Bool,Char,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 5:
            {
            auto df = df_.get_data_by_idx<Bool,Char,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 6:
            {
            auto df = df_.get_data_by_idx<Bool,Char,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 7:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 8:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 9:
            {
            auto df = df_.get_data_by_idx<Bool,Double,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 10:
            {
            auto df = df_.get_data_by_idx<Bool,Double,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 11:
            {
            auto df = df_.get_data_by_idx<Bool,Double,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 12:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 13:
            {
            auto df = df_.get_data_by_idx<Bool,Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 14:
            {
            auto df = df_.get_data_by_idx<Bool,Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 15:
            {
            auto df = df_.get_data_by_idx<Bool,Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 16:
            {
            auto df = df_.get_data_by_idx<Bool,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 17:
            {
            auto df = df_.get_data_by_idx<Bool,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 18:
            {
            auto df = df_.get_data_by_idx<Bool,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 19:
            {
            auto df = df_.get_data_by_idx<Bool,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 20:
            {
            auto df = df_.get_data_by_idx<Bool,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 21:
            {
            auto df = df_.get_data_by_idx<Bool,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 22:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 23:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 24:
            {
            auto df = df_.get_data_by_idx<Char,Double,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 25:
            {
            auto df = df_.get_data_by_idx<Char,Double,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 26:
            {
            auto df = df_.get_data_by_idx<Char,Double,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 27:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 28:
            {
            auto df = df_.get_data_by_idx<Char,Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 29:
            {
            auto df = df_.get_data_by_idx<Char,Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 30:
            {
            auto df = df_.get_data_by_idx<Char,Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 31:
            {
            auto df = df_.get_data_by_idx<Char,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 32:
            {
            auto df = df_.get_data_by_idx<Char,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 33:
            {
            auto df = df_.get_data_by_idx<Char,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 34:
            {
            auto df = df_.get_data_by_idx<Char,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 35:
            {
            auto df = df_.get_data_by_idx<Char,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 36:
            {
            auto df = df_.get_data_by_idx<Char,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 37:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 38:
            {
            auto df = df_.get_data_by_idx<Double,Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 39:
            {
            auto df = df_.get_data_by_idx<Double,Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 40:
            {
            auto df = df_.get_data_by_idx<Double,Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 41:
            {
            auto df = df_.get_data_by_idx<Double,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 42:
            {
            auto df = df_.get_data_by_idx<Double,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 43:
            {
            auto df = df_.get_data_by_idx<Double,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 44:
            {
            auto df = df_.get_data_by_idx<Double,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 45:
            {
            auto df = df_.get_data_by_idx<Double,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 46:
            {
            auto df = df_.get_data_by_idx<Double,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 47:
            {
            auto df = df_.get_data_by_idx<Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 48:
            {
            auto df = df_.get_data_by_idx<Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 49:
            {
            auto df = df_.get_data_by_idx<Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 50:
            {
            auto df = df_.get_data_by_idx<Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 51:
            {
            auto df = df_.get_data_by_idx<Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 52:
            {
            auto df = df_.get_data_by_idx<Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 53:
            {
            auto df = df_.get_data_by_idx<Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 54:
            {
            auto df = df_.get_data_by_idx<Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 55:
            {
            auto df = df_.get_data_by_idx<Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 56:
            {
            auto df = df_.get_data_by_idx<String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
        }
        }
        case 4:
        {
        switch(colMagic_)
        {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float>(idx);
            return DF(std::move(df), colmap_);
            }
            case 2:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 3:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 4:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 5:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 6:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 7:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 8:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 9:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 10:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 11:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 12:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 13:
            {
            auto df = df_.get_data_by_idx<Bool,Char,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 14:
            {
            auto df = df_.get_data_by_idx<Bool,Char,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 15:
            {
            auto df = df_.get_data_by_idx<Bool,Char,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 16:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 17:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 18:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 19:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 20:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 21:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 22:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 23:
            {
            auto df = df_.get_data_by_idx<Bool,Double,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 24:
            {
            auto df = df_.get_data_by_idx<Bool,Double,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 25:
            {
            auto df = df_.get_data_by_idx<Bool,Double,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 26:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 27:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 28:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 29:
            {
            auto df = df_.get_data_by_idx<Bool,Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 30:
            {
            auto df = df_.get_data_by_idx<Bool,Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 31:
            {
            auto df = df_.get_data_by_idx<Bool,Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 32:
            {
            auto df = df_.get_data_by_idx<Bool,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 33:
            {
            auto df = df_.get_data_by_idx<Bool,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 34:
            {
            auto df = df_.get_data_by_idx<Bool,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 35:
            {
            auto df = df_.get_data_by_idx<Bool,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 36:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 37:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 38:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 39:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 40:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 41:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 42:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 43:
            {
            auto df = df_.get_data_by_idx<Char,Double,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 44:
            {
            auto df = df_.get_data_by_idx<Char,Double,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 45:
            {
            auto df = df_.get_data_by_idx<Char,Double,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 46:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 47:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 48:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 49:
            {
            auto df = df_.get_data_by_idx<Char,Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 50:
            {
            auto df = df_.get_data_by_idx<Char,Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 51:
            {
            auto df = df_.get_data_by_idx<Char,Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 52:
            {
            auto df = df_.get_data_by_idx<Char,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 53:
            {
            auto df = df_.get_data_by_idx<Char,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 54:
            {
            auto df = df_.get_data_by_idx<Char,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 55:
            {
            auto df = df_.get_data_by_idx<Char,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 56:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 57:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 58:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 59:
            {
            auto df = df_.get_data_by_idx<Double,Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 60:
            {
            auto df = df_.get_data_by_idx<Double,Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 61:
            {
            auto df = df_.get_data_by_idx<Double,Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 62:
            {
            auto df = df_.get_data_by_idx<Double,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 63:
            {
            auto df = df_.get_data_by_idx<Double,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 64:
            {
            auto df = df_.get_data_by_idx<Double,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 65:
            {
            auto df = df_.get_data_by_idx<Double,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 66:
            {
            auto df = df_.get_data_by_idx<Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 67:
            {
            auto df = df_.get_data_by_idx<Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 68:
            {
            auto df = df_.get_data_by_idx<Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 69:
            {
            auto df = df_.get_data_by_idx<Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 70:
            {
            auto df = df_.get_data_by_idx<Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
        }
        }
        case 5:
        {
        switch(colMagic_)
        {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int>(idx);
            return DF(std::move(df), colmap_);
            }
            case 2:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 3:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 4:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 5:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 6:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 7:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 8:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 9:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 10:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 11:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 12:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 13:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 14:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 15:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 16:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 17:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 18:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 19:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 20:
            {
            auto df = df_.get_data_by_idx<Bool,Char,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 21:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 22:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 23:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 24:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 25:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 26:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 27:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 28:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 29:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 30:
            {
            auto df = df_.get_data_by_idx<Bool,Double,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 31:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 32:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 33:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 34:
            {
            auto df = df_.get_data_by_idx<Bool,Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 35:
            {
            auto df = df_.get_data_by_idx<Bool,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 36:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 37:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 38:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 39:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 40:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 41:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 42:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 43:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 44:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 45:
            {
            auto df = df_.get_data_by_idx<Char,Double,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 46:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 47:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 48:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 49:
            {
            auto df = df_.get_data_by_idx<Char,Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 50:
            {
            auto df = df_.get_data_by_idx<Char,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 51:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 52:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 53:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 54:
            {
            auto df = df_.get_data_by_idx<Double,Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 55:
            {
            auto df = df_.get_data_by_idx<Double,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 56:
            {
            auto df = df_.get_data_by_idx<Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
        }
        }
        case 6:
        {
        switch(colMagic_)
        {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int,String>(idx);
            return DF(std::move(df), colmap_);
            }
            case 2:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 3:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 4:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 5:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 6:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 7:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 8:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 9:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 10:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 11:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 12:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 13:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 14:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 15:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 16:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 17:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 18:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 19:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 20:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 21:
            {
            auto df = df_.get_data_by_idx<Bool,Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 22:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 23:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 24:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 25:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 26:
            {
            auto df = df_.get_data_by_idx<Char,Double,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 27:
            {
            auto df = df_.get_data_by_idx<Char,Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 28:
            {
            auto df = df_.get_data_by_idx<Double,Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
        }
        }
        case 7:
        {
        switch(colMagic_)
        {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int,String,UChar>(idx);
            return DF(std::move(df), colmap_);
            }
            case 2:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int,String,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 3:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 4:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 5:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 6:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 7:
            {
            auto df = df_.get_data_by_idx<Bool,Double,Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            case 8:
            {
            auto df = df_.get_data_by_idx<Char,Double,Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
        }
        }
        case 8:
        {
        switch(colMagic_)
        {
            case 1:
            {
            auto df = df_.get_data_by_idx<Bool,Char,Double,Float,Int,String,UChar,UInt>(idx);
            return DF(std::move(df), colmap_);
            }
            default:
                Util::die("can't return data for column magic {}", colMagic_);
        }
        }
        default:
            Util::die("too many datatypes ({})", nDtypes_);
    }
}

void DF::computeColMagic()
{
    auto dtypes = get_column_types(colmap_);
    nDtypes_ = dtypes.size();
    switch(nDtypes_)
    {
        case 0:
        {
            colMagic_ = 0;
            return;
        }
        case 1:
        {
        if (contains(dtypes,DataType::Bool)) {colMagic_ = 1; return;}
        if (contains(dtypes,DataType::Char)) {colMagic_ = 2; return;}
        if (contains(dtypes,DataType::Double)) {colMagic_ = 3; return;}
        if (contains(dtypes,DataType::Float)) {colMagic_ = 4; return;}
        if (contains(dtypes,DataType::Int)) {colMagic_ = 5; return;}
        if (contains(dtypes,DataType::String)) {colMagic_ = 6; return;}
        if (contains(dtypes,DataType::UChar)) {colMagic_ = 7; return;}
        if (contains(dtypes,DataType::UInt)) {colMagic_ = 8; return;}
        }
        case 2:
        {
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char)){colMagic_ = 1; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double)){colMagic_ = 2; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float)){colMagic_ = 3; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int)){colMagic_ = 4; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::String)){colMagic_ = 5; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::UChar)){colMagic_ = 6; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::UInt)){colMagic_ = 7; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double)){colMagic_ = 8; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float)){colMagic_ = 9; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int)){colMagic_ = 10; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::String)){colMagic_ = 11; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 12; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::UInt)){colMagic_ = 13; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 14; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int)){colMagic_ = 15; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::String)){colMagic_ = 16; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 17; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::UInt)){colMagic_ = 18; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 19; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 20; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 21; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 22; return;}
        if(contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 23; return;}
        if(contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 24; return;}
        if(contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 25; return;}
        if(contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 26; return;}
        if(contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 27; return;}
        if(contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 28; return;}
        }
        case 3:
        {
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double)){colMagic_ = 1; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float)){colMagic_ = 2; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int)){colMagic_ = 3; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::String)){colMagic_ = 4; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar)){colMagic_ = 5; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UInt)){colMagic_ = 6; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 7; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int)){colMagic_ = 8; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String)){colMagic_ = 9; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 10; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UInt)){colMagic_ = 11; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 12; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 13; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 14; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 15; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 16; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 17; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 18; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 19; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 20; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 21; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 22; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int)){colMagic_ = 23; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String)){colMagic_ = 24; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 25; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UInt)){colMagic_ = 26; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 27; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 28; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 29; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 30; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 31; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 32; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 33; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 34; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 35; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 36; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 37; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 38; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 39; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 40; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 41; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 42; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 43; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 44; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 45; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 46; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 47; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 48; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 49; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 50; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 51; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 52; return;}
        if(contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 53; return;}
        if(contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 54; return;}
        if(contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 55; return;}
        if(contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 56; return;}
        }
        case 4:
        {
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float)){colMagic_ = 1; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int)){colMagic_ = 2; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String)){colMagic_ = 3; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar)){colMagic_ = 4; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UInt)){colMagic_ = 5; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 6; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 7; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 8; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 9; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 10; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 11; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 12; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 13; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 14; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 15; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 16; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 17; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 18; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 19; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 20; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 21; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 22; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 23; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 24; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 25; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 26; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 27; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 28; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 29; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 30; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 31; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 32; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 33; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 34; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 35; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 36; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 37; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 38; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 39; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 40; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 41; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 42; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 43; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 44; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 45; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 46; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 47; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 48; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 49; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 50; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 51; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 52; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 53; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 54; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 55; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 56; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 57; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 58; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 59; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 60; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 61; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 62; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 63; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 64; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 65; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 66; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 67; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 68; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 69; return;}
        if(contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 70; return;}
        }
        case 5:
        {
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int)){colMagic_ = 1; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String)){colMagic_ = 2; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar)){colMagic_ = 3; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UInt)){colMagic_ = 4; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 5; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 6; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 7; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 8; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 9; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 10; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 11; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 12; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 13; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 14; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 15; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 16; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 17; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 18; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 19; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 20; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 21; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 22; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 23; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 24; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 25; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 26; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 27; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 28; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 29; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 30; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 31; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 32; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 33; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 34; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 35; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 36; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 37; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 38; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 39; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 40; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 41; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 42; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 43; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 44; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 45; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 46; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 47; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 48; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 49; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 50; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 51; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 52; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 53; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 54; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 55; return;}
        if(contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 56; return;}
        }
        case 6:
        {
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String)){colMagic_ = 1; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar)){colMagic_ = 2; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UInt)){colMagic_ = 3; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 4; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 5; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 6; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 7; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 8; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 9; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 10; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 11; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 12; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 13; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 14; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 15; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 16; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 17; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 18; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 19; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 20; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 21; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 22; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 23; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 24; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 25; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 26; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 27; return;}
        if(contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 28; return;}
        }
        case 7:
        {
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar)){colMagic_ = 1; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UInt)){colMagic_ = 2; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 3; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 4; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 5; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Char) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 6; return;}
        if(contains(dtypes,DataType::Bool) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 7; return;}
        if(contains(dtypes,DataType::Char) && contains(dtypes,DataType::Double) && contains(dtypes,DataType::Float) && contains(dtypes,DataType::Int) && contains(dtypes,DataType::String) && contains(dtypes,DataType::UChar) && contains(dtypes,DataType::UInt)){colMagic_ = 8; return;}
        }
        case 8:
        {
            colMagic_ = 1;
            return;
        }
        default:
        {
            throw std::runtime_error("too many datatypes in DF");
        }
    }
}