cmake_minimum_required(VERSION 3.25)
project(learningCPP)

set(CMAKE_CXX_STANDARD 17)

add_executable(learningCPP main.cpp)

find_package(dpp CONFIG REQUIRED)
target_link_libraries(learningCPP PRIVATE dpp::dpp)


