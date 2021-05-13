#include <vector>

#include "def.h"

constexpr size_t N = 3;

// int main()
// {
    // DataFrame         df;
    // // std::vector<int>    &col0 =
    // //     df.create_column<int>(static_cast<const char *>("col_name"));

    // // std::vector<int>            intvec = { 1, 2, 3, 4, 5 };
    // std::vector<double>         dblvec =
    //     { 1.2345, 2.2345, 3.2345, 4.2345, 5.2345 };
    // // std::vector<double>         dblvec2 =
    // //     { 0.998, 0.3456, 0.056, 0.15678, 0.00345, 0.923, 0.06743, 0.1 };
    // // std::vector<std::string>    strvec =
    // //     { "Col_name", "Col_name", "Col_name", "Col_name", "Col_name" };
    // std::vector<unsigned long>  idxvec =
    //     { 1UL, 2UL, 3UL, 4UL, 5UL, 6UL, 7UL, 8UL, 9UL };
    // // std::vector<unsigned long>  xulgvec = idxvec;
    // // const size_t                total_count =
    // //     idxvec.size() +
    // //     intvec.size() +
    // //     dblvec.size() +
    // //     dblvec2.size() +
    // //     strvec.size() +
    // //     xulgvec.size() +
    // //     9;  // NaN inserterd

    // // DataFrame::size_type  rc =
    // //     df.load_data(std::move(idxvec),
    // //                 std::make_pair("int_col", intvec),
    // //                 std::make_pair("dbl_col", dblvec),
    // //                 std::make_pair("dbl_col_2", dblvec2),
    // //                 std::make_pair("str_col", strvec),
    // //                 std::make_pair("ul_col", xulgvec));

    // // assert(rc == 48);

    // df.load_index(idxvec.begin(), idxvec.end());
    // // df.load_column<int>("int_col", { intvec.begin(), intvec.end() },
    // //                     hmdf::nan_policy::pad_with_nans);
    // // df.load_column<std::string>("str_col", { strvec.begin(), strvec.end() },
    // //                             hmdf::nan_policy::pad_with_nans);
    // df.load_column<double>("dbl_col", { dblvec.begin(), dblvec.end() },
    //                     hmdf::nan_policy::dont_pad_with_nans);
    // // df.load_column<double>("dbl_col_2", { dblvec2.begin(), dblvec2.end() },
    // //                     hmdf::nan_policy::dont_pad_with_nans);

    // // df.append_column<std::string>("str_col", "Additional column");
    // df.append_column("dbl_col", 10.56);
    // df.write<std::ostream, double>(std::cout, hmdf::io_format::csv2); 
// }

int main()
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
    df.write<std::ostream, int, std::string>(std::cout, hmdf::io_format::csv2); 
}