cmake_minimum_required(VERSION 3.8)
project(Minority_Game_Programming)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Evolutionary_Minority_Game_Functions.h EMG_API.cpp)
add_executable(Minority_Game_Programming ${SOURCE_FILES}  Evolutionary_Minority_Game_Functions.h EMG_API.cpp)