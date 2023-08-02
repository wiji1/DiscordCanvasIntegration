cmake_minimum_required(VERSION 3.25)
project(learningCPP)

set(CMAKE_CXX_STANDARD 17)
#add_subdirectory(libs/DPP)

add_executable(learningCPP main.cpp)

# Linking libraries
target_link_libraries(learningCPP dpp)

# Specify includes
target_include_directories(learningCPP PRIVATE libs/DPP/include)

find_package(dpp CONFIG REQUIRED)
#target_link_libraries(learningCPP PRIVATE dpp::dpp)


#cmake_minimum_required(VERSION 3.25)
#project(learningCPP)
#
#set(CMAKE_CXX_STANDARD 17)
#
#add_executable(learningCPP main.cpp)
#
#find_package(dpp CONFIG REQUIRED)
#target_link_libraries(learningCPP PRIVATE dpp::dpp)




