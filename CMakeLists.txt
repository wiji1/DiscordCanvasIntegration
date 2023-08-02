cmake_minimum_required(VERSION 3.25)
project(learningCPP)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(libs/DPP)

add_executable(learningCPP main.cpp)

# Linking libraries
target_link_libraries(learningCPP
        dpp
        # Add any other libs you want to use here
        )

# Specify includes
target_include_directories(learningCPP PRIVATE
        libs/DPP/include
        # Remember to add the include directories of any other libraries too
        )


find_package(dpp CONFIG REQUIRED)
#target_link_libraries(learningCPP PRIVATE dpp::dpp)


