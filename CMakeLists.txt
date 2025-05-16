# CMakeList.txt : CMake project for chess3d, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)
project ("chess3d")

# Add source to this project's executable.
add_executable (chess3d "chess3d.cpp" "chess3d.hpp" "logic.cpp" "logic.hpp" )

# TODO: Add tests and install targets if needed.

#add_subdirectory(pgnp/libs/chess-move-interface)
#add_subdirectory(pgnp)

#target_link_libraries(chess3d pgnp ChessMoveInterface)
#target_link_libraries(chess3d pgnp)
