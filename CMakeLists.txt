# CMakeList.txt : CMake project for chess3d, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)
project ("chess3d")

# Add source to this project's executable.
file(GLOB chess3d_SRC
     "src/*.hpp"
     "src/*.cpp"
)

add_executable(chess3d ${chess3d_SRC})