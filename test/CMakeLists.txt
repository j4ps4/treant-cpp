set(test_bin1 df_test)
add_executable(${test_bin1} df_test.cpp)
target_link_libraries(${test_bin1} Catch2 Credit)
target_include_directories(${test_bin1} PRIVATE ../${src_dir})
target_compile_features(${test_bin1} PRIVATE cxx_std_20)

add_test(NAME df COMMAND ${test_bin1})

add_custom_command(
    TARGET ${test_bin1} PRE_BUILD
    COMMAND ./fix-test.sh
)