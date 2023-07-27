cmake_minimum_required(VERSION 3.25)
project(learningCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")

add_executable(learningCPP main.cpp Account.cpp Account.h Savings_Account.cpp Savings_Account.h Account_Util.cpp Account_Util.h Checking_Account.cpp Checking_Account.h Trust_Account.cpp Trust_Account.h I_Printable.cpp I_Printable.h)
