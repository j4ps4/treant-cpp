cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
elseif(NOT CMAKE_BUILD_TYPE STREQUAL Debug AND 
       NOT CMAKE_BUILD_TYPE STREQUAL Release)
    message(FATAL_ERROR "Invalid build type!")
endif()

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

project(treant-cpp CXX)
set(src_dir "src")
set(df_dir ${src_dir}/DF2)
set(main_cpp ${src_dir}/main.cpp)
set(target_name treant)

set(main_src ${main_cpp})
set(credit_src ${src_dir}/dataset/Credit.cpp ${src_dir}/AttackerRule.cpp)
#set(covertype_src ${src_dir}/dataset/Covertype.cpp ${src_dir}/AttackerRule.cpp)
set(har_src ${src_dir}/dataset/HAR.cpp ${src_dir}/AttackerRule.cpp)
#file(GLOB_RECURSE DF_src CONFIGURE_DEPENDS ${src_dir}/DF2/*.cpp)

option(ENABLE_MY_TESTING "Enable My Tests" ON)

# include(FindPkgConfig)
# pkg_check_modules(z REQUIRED zlib)

find_package(Eigen3 CONFIG REQUIRED)

include(cmake/CPM.cmake)
CPMAddPackage(
    GITHUB_REPOSITORY jarro2783/cxxopts
    VERSION 2.2.1
    OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES")
CPMAddPackage("gh:USCiLab/cereal@1.3.0")
CPMAddPackage("gh:fmtlib/fmt#7.1.3")
CPMAddPackage("gh:dropbox/json11@1.0.0")
CPMAddPackage("gh:catchorg/Catch2@2.5.0")
CPMAddPackage("gh:stevengj/nlopt@2.7.0")
set(CPM_USE_LOCAL_PACKAGES)

add_executable(${target_name} ${main_src})
add_library(Credit SHARED ${credit_src})
#add_library(Covertype SHARED ${covertype_src})
add_library(HAR SHARED ${har_src})

target_link_libraries(Credit bz2 fmt json11 cereal nlopt pthread)
target_link_libraries(HAR bz2 fmt json11 cereal nlopt pthread)
target_link_libraries(${target_name} Credit HAR cxxopts fmt)

set(lib_cxx_flags -Og -pg -Wall -Wconversion -Wno-unused-variable -Wno-unused-function -Wno-deprecated)

# TODO find a better way
target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:${lib_cxx_flags}>)
target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-O3 -Wno-deprecated -mtune=native -march=native>)
target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-pg>)
target_compile_options(Credit PRIVATE $<$<CONFIG:Debug>:${lib_cxx_flags}>)
target_compile_options(Credit PRIVATE $<$<CONFIG:Release>:-O3 -Wno-deprecated -mtune=native -march=native>)
target_link_options(Credit PRIVATE $<$<CONFIG:Debug>:-pg>)
#target_compile_options(Covertype PRIVATE $<$<CONFIG:Debug>:${lib_cxx_flags}>)
#target_compile_options(Covertype PRIVATE $<$<CONFIG:Release>:-O3 -mtune=native -march=native>)
#target_link_options(Covertype PRIVATE $<$<CONFIG:Debug>:-pg>)
target_compile_options(HAR PRIVATE $<$<CONFIG:Debug>:${lib_cxx_flags}>)
target_compile_options(HAR PRIVATE $<$<CONFIG:Release>:-O3 -Wno-deprecated -mtune=native -march=native>)
target_link_options(HAR PRIVATE $<$<CONFIG:Debug>:-pg>)
target_compile_features(${target_name} PRIVATE cxx_std_20)
target_compile_features(Credit PRIVATE cxx_std_20)
#target_compile_features(Covertype PRIVATE cxx_std_20)
target_compile_features(HAR PRIVATE cxx_std_20)

message("Generated with ${CMAKE_BUILD_TYPE}")
