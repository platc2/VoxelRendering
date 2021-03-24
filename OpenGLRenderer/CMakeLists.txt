# File: OpenGLRenderer/CMakeLists.txt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

add_library(OpenGLRenderer
	${CMAKE_CURRENT_SOURCE_DIR}/source/OpenGLRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Mesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Material.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/RenderPass.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/VertexAttribute.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Texture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Window.cpp
)
add_library(Voxel::OpenGLRenderer ALIAS OpenGLRenderer)

target_include_directories(OpenGLRenderer
	PUBLIC
		$<INSTALL_INTERFACE:include/OpenGLRenderer>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_compile_features(OpenGLRenderer PRIVATE cxx_std_11)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)

target_link_libraries(OpenGLRenderer
	PUBLIC
		glm::glm imgui::imgui
	PRIVATE
		OpenGL::GL GLEW::GLEW
)

file(GLOB PublicHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB PrivateHeaders ${CMAKE_CURRENT_SOURCE_DIR}/source/*.h ${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp)
target_sources(OpenGLRenderer PRIVATE ${PublicHeaders} ${PrivateHeaders})

target_compile_options(OpenGLRenderer PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall
		-Wextra
		-pedantic>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W3>
)


include(GNUInstallDirs)
install(TARGETS OpenGLRenderer
	EXPORT OpenGLRenderer-targets
	LIBRARY DESTINATION "{CMAKE_INSTALL_LIBDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/OpenGLRenderer/")

install(EXPORT OpenGLRenderer-targets
	FILE
		"OpenGLRendererTargets.cmake"
	NAMESPACE
		Voxel::
	DESTINATION
		"${CMAKE_INSTALL_LIBDIR}/cmake/OpenGLRenderer"
)

configure_file(cmake/OpenGLRendererConfig.cmake.in OpenGLRendererConfig.cmake @ONLY)
install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/OpenGLRendererConfig.cmake"
	DESTINATION
		"${CMAKE_INSTALL_LIBDIR}/cmake/OpenGLRenderer"
)
