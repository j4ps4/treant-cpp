cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
elseif(NOT CMAKE_BUILD_TYPE STREQUAL Debug AND 
       NOT CMAKE_BUILD_TYPE STREQUAL Release)
    message(FATAL_ERROR "Invalid build type!")
endif()

project(treant-cpp CXX)
set(source_dir "src")
set(test_dir "test")
set(main_cpp ${source_dir}/main.cpp)
set(target_name treant)
set(df_test_bin df_test)
set(json_test_bin json_test)

file(GLOB_RECURSE src CONFIGURE_DEPENDS ${source_dir}/*.cpp)

# include(FindPkgConfig)
# pkg_check_modules(z REQUIRED zlib)

find_package(NLopt CONFIG REQUIRED)

include(cmake/CPM.cmake)
CPMAddPackage("gh:jarro2783/cxxopts@2.2.1")
CPMAddPackage("gh:USCiLab/cereal@1.3.0")
CPMAddPackage("gh:fmtlib/fmt#7.1.3")
CPMAddPackage("gh:hosseinmoein/DataFrame#1.17.0")
CPMAddPackage("gh:dropbox/json11@1.0.0")
set(CPM_USE_LOCAL_PACKAGES)

add_executable(${target_name} ${main_cpp} ${src})
add_executable(${df_test_bin} ${test_dir}/df_test.cpp)
add_executable(${json_test_bin} ${test_dir}/json_test.cpp)

target_link_libraries(${target_name} cxxopts fmt nlopt bz2 DataFrame json11)
target_include_directories(${df_test_bin} PRIVATE ${source_dir})
target_link_libraries(${df_test_bin} DataFrame cereal)
target_link_libraries(${json_test_bin} json11)

target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-Og -Wall -Wno-unused-variable -Woverloaded-virtual>)
target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-O3 -mtune=native -march=native>)
target_compile_features(${target_name} PRIVATE cxx_std_17)
target_compile_features(${df_test_bin} PRIVATE cxx_std_17)
target_compile_features(${json_test_bin} PRIVATE cxx_std_17)

message("Generated with ${CMAKE_BUILD_TYPE}")