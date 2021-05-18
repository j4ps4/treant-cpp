#include <vector>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

#include "def.h"

constexpr size_t N = 3;

void input(const char* fn)
{
    std::vector<IdxT> idx(N);
    std::iota(idx.begin(), idx.end(), 0);
    DataFrame df;
    df.load_index(std::move(idx));

    std::ifstream is(fn, std::ios::binary);
    cereal::BinaryInputArchive iarchive(is);
    std::vector<int> vec1;
    std::vector<std::string> vec2;
    iarchive(vec1, vec2);

    df.load_column("intvals", std::move(vec1), hmdf::nan_policy::dont_pad_with_nans);
    df.load_column("stringvals", std::move(vec2), hmdf::nan_policy::dont_pad_with_nans);
    df.write<std::ostream, int, std::string>(std::cout, hmdf::io_format::csv2); 
}

void output(const char* fn)
{
    std::vector<int> vals;
    std::vector<std::string> vals2;
    vals.reserve(N);
    vals2.reserve(N);

    std::vector<IdxT> idx(N);
    std::iota(idx.begin(), idx.end(), 0);

    DataFrame df;
    df.load_index(std::move(idx));
    df.load_column("intvals", std::move(vals), hmdf::nan_policy::dont_pad_with_nans);
    df.load_column("stringvals", std::move(vals2), hmdf::nan_policy::dont_pad_with_nans);
    df.append_column<int>("intvals", 10);
    df.append_column<std::string>("stringvals", "hello");
    // df.get_column<int>((size_t)0).push_back(10);
    // df.get_column<int>((size_t)0).push_back(20);

    std::ofstream os(fn, std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    auto& vec1 = df.get_column<int>("intvals");
    auto& vec2 = df.get_column<std::string>("stringvals");
    archive(vec1, vec2);
}

int main(int argc, char** argv)
{
    const char* fn = "out.cereal";
    if (argc > 1)
        output(fn);
    else
        input(fn);

}