cmake_minimum_required(VERSION 3.10)
project(optional_test LANGUAGES CXX)

include(AddGoogleTest)
add_executable(optional_test optional_test.cpp)
target_include_directories(optional_test PRIVATE ${PROJECT_SOURCE_DIR}/../palkin)

add_gtest(optional_test)