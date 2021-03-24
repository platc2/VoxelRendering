cmake_minimum_required(VERSION 3.1.2 FATAL_ERROR)
project(VoxelRendering
	LANGUAGES CXX
	VERSION 0.1.0
)

# set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_PREFIX_PATH}")

# Libraries
add_subdirectory(Voxel)
add_subdirectory(OpenGLRenderer)

# Executables
add_subdirectory(HelloTriangle)
add_subdirectory(Cube)
add_subdirectory(TexturedCube)
add_subdirectory(SobelCube)
add_subdirectory(VoxelWorld)

add_subdirectory(Planet)

