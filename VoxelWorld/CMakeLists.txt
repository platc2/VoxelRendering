# File: VoxelWorld/CMakeLists.txt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Dependencies
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(stb REQUIRED)
find_package(fmt REQUIRED)
find_package(imgui REQUIRED)
find_package(Assimp REQUIRED)
# TODO Remove
find_package(OpenGL REQUIRED)

set(GLSLShaderPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/glsl)
set(SPIRVShaderSource ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/spir-v)
set(SPIRVShaderPath ${CMAKE_CURRENT_BINARY_DIR})
set(MeshPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/meshes)
set(TexturePath ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures)

add_executable(VoxelWorld
	${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/RenderLoop.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/ShaderProgram.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Mesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Texture.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/RenderChunk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/RenderChunkGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/WorldGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/WorldRenderer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/imgui_impl_glfw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/imgui_impl_opengl3.cpp
)

# TODO Remove OpenGL & GLEW
target_link_libraries(VoxelWorld PRIVATE OpenGL::GL GLEW::GLEW glfw::glfw fmt::fmt stb::stb imgui::imgui Assimp::Assimp Voxel::Voxel Voxel::OpenGLRenderer)

target_compile_definitions(VoxelWorld PRIVATE
	# Path for OpenGL shaders
	GLSL_SHADER_PATH="${GLSLShaderPath}"
	# Path for Vulkan (Spir-V) shaders
	SPIRV_SHADER_PATH="${SPIRVShaderPath}"
	# Path for meshes
	MESH_PATH="${MeshPath}"
	# Path for textures
	TEXTURE_PATH="${TexturePath}"
	# Enable GLM experimental features
	GLM_ENABLE_EXPERIMENTAL=1
	# Workaround for broken detection of clang c++11 stdlib
	# https://github.com/g-truc/glm/issues/620
	GLM_FORCE_CXX11=1
)

target_compile_features(VoxelWorld PRIVATE cxx_std_11)

target_include_directories(VoxelWorld PRIVATE include)
target_include_directories(VoxelWorld PRIVATE external)

# Add header files to IDE
file(GLOB_RECURSE Headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
target_sources(VoxelWorld PRIVATE ${Headers})


add_custom_target(copy-resources ALL
    COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/VoxelWorld/resources)

# Setup GLSL shaders, i.e. add them to the IDE
set(GLSLShaders
	${GLSLShaderPath}/cube.vert
	${GLSLShaderPath}/cube.frag
)
foreach(GLSLShader IN LISTS GLSLShaders)
	string(REGEX REPLACE ${GLSLShaderPath} "" GLSLShaderFile ${GLSLShader})
	get_filename_component(Path ${GLSLShaderFile} DIRECTORY)
	source_group(Shaders\\GLSL\\${Path} FILES ${GLSLShader})
endforeach(GLSLShader)
target_sources(VoxelWorld PRIVATE ${GLSLShaders})

target_compile_options(VoxelWorld PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall
		-Wextra
		-pedantic
		-Wconversion
		-Wduplicated-cond
		-Wduplicated-branches
		-Wlogical-op
		-Wrestrict
		-Wnull-dereference
		-Wold-style-cast
		-Wuseless-cast
		-Wdouble-promotion
		-Wformat=2>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W3>
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
