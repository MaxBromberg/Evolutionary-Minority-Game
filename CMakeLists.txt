cmake_minimum_required(VERSION 3.8)
project(Minority_Game_Programming)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Evolutionary_Minority_Game.cpp Agents and Evolution Strategies.cpp )

set(API_SOURCE Evolutionary_Minority_Game.cpp "Agents and Evolution Strategies.cpp" Minority_Game_Utilities.cpp )
set(TEST_SOURCE Evolutionary_Minority_Game_Test.cpp "Agents and Evolution Strategies.cpp" Minority_Game_Utilities.cpp )

add_executable(Minority_Game_Programming main.cpp ${API_SOURCE} )
add_executable(Minority_Game_UnitTests test_main.cpp ${API_SOURCE} ${TEST_SOURCE} )
