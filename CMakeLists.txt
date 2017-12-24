cmake_minimum_required(VERSION 3.8)
project(Minority_Game_Programming)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ESimulation_Functions.cpp volutionary_Minority_Game.h .cpp debug_utilities.h Intermediary_Function_Tests.h)

set(API_SOURCE Evolutionary_Minority_Game.cpp)
set(TEST_SOURCE Evolutionary_Minority_Game_Test.cpp Intermediary_Function_Tests.cpp)

add_executable(Minority_Game_Programming main.cpp ${API_SOURCE} Simulation_Functions.cpp)
add_executable(Minority_Game_UnitTests test_main.cpp ${API_SOURCE} ${TEST_SOURCE})