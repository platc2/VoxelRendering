# File: Voxel/CMakeLists.txt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

add_library(Voxel
	${CMAKE_CURRENT_SOURCE_DIR}/source/FileUtilities.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Trackball.cpp
)
add_library(Voxel::Voxel ALIAS Voxel)

target_include_directories(Voxel
	PUBLIC
		$<INSTALL_INTERFACE:include/Voxel>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_compile_features(Voxel PRIVATE cxx_std_11)

find_package(glm REQUIRED)
find_package(stb REQUIRED)

target_link_libraries(Voxel
	PUBLIC
		glm::glm
	PRIVATE
		stb::stb
)

file(GLOB PublicHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB PrivateHeaders ${CMAKE_CURRENT_SOURCE_DIR}/source/*.h ${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp)
target_sources(Voxel PRIVATE ${PublicHeaders} ${PrivateHeaders})

target_compile_options(Voxel PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall
		-Wextra
		-pedantic>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W3>
)


include(GNUInstallDirs)
install(TARGETS Voxel
	EXPORT Voxel-targets
	LIBRARY DESTINATION "{CMAKE_INSTALL_LIBDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/Voxel/")

install(EXPORT Voxel-targets
	FILE
		"VoxelTargets.cmake"
	NAMESPACE
		Voxel::
	DESTINATION
		"${CMAKE_INSTALL_LIBDIR}/cmake/Voxel"
)

configure_file(cmake/VoxelConfig.cmake.in VoxelConfig.cmake @ONLY)
install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/VoxelConfig.cmake"
	DESTINATION
		"${CMAKE_INSTALL_LIBDIR}/cmake/Voxel"
)
