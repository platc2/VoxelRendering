# File: Planet/CMakeLists.txt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Dependencies
find_package(glfw3 REQUIRED)
find_package(stb REQUIRED)
find_package(GLEW REQUIRED)


set(GLSLShaderPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/glsl)
set(SPIRVShaderPath ${CMAKE_CURRENT_BINARY_DIR})
set(TexturePath ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures)


add_executable(Planet
	${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
)

target_link_libraries(Planet PRIVATE GLEW::GLEW glfw::glfw stb::stb Voxel::Voxel Voxel::OpenGLRenderer)

target_compile_definitions(Planet PRIVATE
	# Path for OpenGL shaders
	GLSL_SHADER_PATH="${GLSLShaderPath}"
	# Path for textures
	TEXTURE_PATH="${TexturePath}"
)

target_compile_features(Planet PRIVATE cxx_std_14)

target_include_directories(Planet PRIVATE include)

# Add header files to IDE
file(GLOB Headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
target_sources(Planet PRIVATE ${Headers})


# Setup GLSL shaders, i.e. add them to the IDE
set(GLSLShaders
	${GLSLShaderPath}/basic/basic.vert
	${GLSLShaderPath}/basic/basic.frag
	${GLSLShaderPath}/basic/basic.geom
	${GLSLShaderPath}/basic/basic.tesc
	${GLSLShaderPath}/basic/basic.tese
)
foreach(GLSLShader IN LISTS GLSLShaders)
	string(REGEX REPLACE ${GLSLShaderPath} "" GLSLShaderFile ${GLSLShader})
	get_filename_component(Path ${GLSLShaderFile} DIRECTORY)
	source_group(Shaders\\GLSL\\${Path} FILES ${GLSLShader})
endforeach(GLSLShader)
target_sources(Planet PRIVATE ${GLSLShaders})

target_compile_options(Planet PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall
		-Wextra
		-pedantic>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W3>
)
