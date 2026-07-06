set(CMAKE_CXX_STANDARD 17)
project("chess3d")

set(ASSIMP_ROOT "C:/Users/snowi/source/repos/assimp")
set(CPP_LIBS "C:/Users/snowi/Documents/cpp_libs")

# Collect all source files
file(GLOB_RECURSE CHESS3D_SOURCES "src/*.cpp")
file(GLOB_RECURSE CHESS3D_HEADERS "src/*.hpp")

# Collect all external source files
file(GLOB_RECURSE EXTERN_SOURCES "extern/*.c" "extern/*.cpp")

# Create executable with external sources included
add_executable(chess3d ${CHESS3D_SOURCES} ${CHESS3D_HEADERS} ${EXTERN_SOURCES})

# Include directories
target_include_directories(chess3d PRIVATE
    "extern"
    "${ASSIMP_ROOT}/build/include"
    "${ASSIMP_ROOT}/include"
    "${CPP_LIBS}/Include"
)

# Link libraries
target_link_libraries(chess3d PRIVATE
    "${CPP_LIBS}/Libs/glfw3.lib"
    "${ASSIMP_ROOT}/build/lib/Release/assimp-vc143-mt.lib"
    opengl32
)

# C:\Users\snowi\source\repos\assimp\build\include;
# C:\Users\snowi\source\repos\assimp\include;
# C:\Users\snowi\Documents\cpp_libs\Include;
# $(IncludePath)

# C:\Users\snowi\source\repos\assimp\build\lib\Release;
# C:\Users\snowi\source\repos\assimp\lib;
# C:\Users\snowi\Documents\cpp_libs\Libs;
# $(LibraryPath)

# glfw3.lib;
# opengl32.lib;
# assimp-vc143-mt.lib;
# %(AdditionalDependencies)