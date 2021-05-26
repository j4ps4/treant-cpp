cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
elseif(NOT CMAKE_BUILD_TYPE STREQUAL Debug AND 
       NOT CMAKE_BUILD_TYPE STREQUAL Release)
    message(FATAL_ERROR "Invalid build type!")
endif()

project(treant-cpp CXX)
set(src_dir "src")
set(df_dir ${src_dir}/DF2)
set(main_cpp ${src_dir}/main.cpp)
set(target_name treant)

set(main_src ${main_cpp})
#file(GLOB_RECURSE DF_src CONFIGURE_DEPENDS ${src_dir}/DF2/*.cpp)

option(ENABLE_MY_TESTING "Enable My Tests" ON)

# include(FindPkgConfig)
# pkg_check_modules(z REQUIRED zlib)

find_package(NLopt CONFIG REQUIRED)

include(cmake/CPM.cmake)
CPMAddPackage(
    GITHUB_REPOSITORY jarro2783/cxxopts
    VERSION 2.2.1
    OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES")
CPMAddPackage("gh:USCiLab/cereal@1.3.0")
CPMAddPackage("gh:fmtlib/fmt#7.1.3")
CPMAddPackage(
    GITHUB_REPOSITORY hosseinmoein/DataFrame
    GIT_TAG 1.17.0
    OPTIONS "ENABLE_TESTING OFF")
CPMAddPackage("gh:dropbox/json11@1.0.0")
CPMAddPackage("gh:catchorg/Catch2@2.5.0")
set(CPM_USE_LOCAL_PACKAGES)

add_executable(${target_name} ${main_src})
#add_library(DF STATIC ${DF_src})

#target_link_libraries(DF bz2 DataFrame fmt)
target_link_libraries(${target_name} cxxopts fmt nlopt bz2 json11)

target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-Og -pg -Wall -Wno-unused-variable -Woverloaded-virtual>)
target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-O3 -mtune=native -march=native>)
target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-pg>)
#target_compile_options(DF PRIVATE $<$<CONFIG:Debug>:-Og -pg -Wall -Wno-unused-variable -Woverloaded-virtual>)
#target_compile_options(DF PRIVATE $<$<CONFIG:Release>:-O3 -mtune=native -march=native>)
#target_link_options(DF PRIVATE $<$<CONFIG:Debug>:-pg>)
target_compile_features(${target_name} PRIVATE cxx_std_17)
#target_compile_features(DF PRIVATE cxx_std_17)

if(ENABLE_MY_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

message("Generated with ${CMAKE_BUILD_TYPE}")