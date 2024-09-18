# CMakeList.txt : CMake project for chess3d, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("chess3d")

# Add source to this project's executable.
add_executable (chess3d "chess3d.cpp" "chess3d.hpp" "logic.cpp" "logic.hpp")

# TODO: Add tests and install targets if needed.
