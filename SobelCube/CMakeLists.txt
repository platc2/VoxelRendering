# File: SobelCube/CMakeLists.txt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Dependencies
find_package(glfw3 REQUIRED)
find_package(stb REQUIRED)
find_package(imgui REQUIRED)
# TODO Remove
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)


set(GLSLShaderPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/glsl)
set(SPIRVShaderPath ${CMAKE_CURRENT_BINARY_DIR})
set(TexturePath ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures)


add_executable(SobelCube
	${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
)

target_link_libraries(SobelCube PRIVATE OpenGL::GL GLEW::GLEW glfw::glfw imgui::imgui stb::stb Voxel::Voxel Voxel::OpenGLRenderer)

target_compile_definitions(SobelCube PRIVATE
	# Path for OpenGL shaders
	GLSL_SHADER_PATH="${GLSLShaderPath}"
	# Path for textures
	TEXTURE_PATH="${TexturePath}"
)

target_compile_features(SobelCube PRIVATE cxx_std_11)

target_include_directories(SobelCube PRIVATE include)

# Add header files to IDE
file(GLOB Headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
target_sources(SobelCube PRIVATE ${Headers})


# Setup GLSL shaders, i.e. add them to the IDE
set(GLSLShaders
	${GLSLShaderPath}/basic/basic.vert
	${GLSLShaderPath}/basic/basic.frag
	${GLSLShaderPath}/cube/cube.vert
	${GLSLShaderPath}/cube/cube.frag
)
foreach(GLSLShader IN LISTS GLSLShaders)
	string(REGEX REPLACE ${GLSLShaderPath} "" GLSLShaderFile ${GLSLShader})
	get_filename_component(Path ${GLSLShaderFile} DIRECTORY)
	source_group(Shaders\\GLSL\\${Path} FILES ${GLSLShader})
endforeach(GLSLShader)
target_sources(SobelCube PRIVATE ${GLSLShaders})

target_compile_options(SobelCube PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall
		-Wextra
		-pedantic>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W3>
)

