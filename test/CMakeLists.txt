add_executable(json_test json_test.cpp)
target_link_libraries(json_test json11 Catch2)
target_compile_features(json_test PRIVATE cxx_std_17)

add_test(NAME json COMMAND json_test)