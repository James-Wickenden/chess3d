cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project("chess3d")

set(ASSIMP_ROOT "" CACHE PATH "Path to Assimp")
set(CPP_LIBS "" CACHE PATH "Path to third-party libraries")

# Collect all source files (EXCLUDING chess3d.cpp - the entry point)
file(GLOB_RECURSE CHESS3D_SOURCES "src/*.cpp")
file(GLOB_RECURSE CHESS3D_HEADERS "src/*.hpp")
# Collect all test files
file(GLOB_RECURSE CHESS3D_TEST_SOURCES "test/*.cpp")

# Remove chess3d.cpp from library sources
list(FILTER CHESS3D_SOURCES EXCLUDE REGEX "chess3d\\.cpp$")

# Collect all external source files
file(GLOB_RECURSE EXTERN_SOURCES "extern/*.c" "extern/*.cpp")

# Create a library from source files (shared between main executable and tests)
add_library(chess3d_lib STATIC ${CHESS3D_SOURCES} ${EXTERN_SOURCES})

# Configure library include directories
target_include_directories(chess3d_lib PUBLIC
    "src"
    "extern"
    "${ASSIMP_ROOT}/build/include"
    "${ASSIMP_ROOT}/include"
    "${CPP_LIBS}/Include"
)

# Link libraries for the chess3d_lib
target_link_libraries(chess3d_lib PUBLIC
    "${CPP_LIBS}/Libs/glfw3.lib"
    "${ASSIMP_ROOT}/build/lib/Release/assimp-vc143-mt.lib"
    opengl32
)

# Create main executable (uses only chess3d.cpp as entry point)
add_executable(chess3d src/chess3d.cpp)
target_link_libraries(chess3d PRIVATE chess3d_lib)

# Install googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Create test executable
add_executable(chess3d_tests ${CHESS3D_TEST_SOURCES})

# Link both the library and GTest to the test executable
target_link_libraries(chess3d_tests PRIVATE
    chess3d_lib
    GTest::gtest_main
)

# Include directories for tests
target_include_directories(chess3d_tests PRIVATE
    "src"
    "extern"
)

include(GoogleTest)
gtest_discover_tests(chess3d_tests)
