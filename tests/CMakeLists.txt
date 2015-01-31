cmake_minimum_required (VERSION 2.6)
project ("JSON Parser")
add_executable(whitebox_json_parser whitebox_json_parser.cc)
add_executable(whitebox_subbuffer whitebox_subbuffer.cc)
add_executable(whitebox_aton whitebox_aton.cc)

include_directories(BEFORE ../include)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fprofile-arcs -ftest-coverage")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")


