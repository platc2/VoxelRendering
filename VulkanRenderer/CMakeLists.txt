# File: Library/CMakeLists.txt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

add_library(VkRenderer
	${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
)
add_library(Voxel::VulkanRenderer ALIAS VkRenderer)

target_include_directories(VkRenderer
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/source
)

# target_compile_options(VkRenderer PRIVATE -Werror)

target_compile_features(VkRenderer PRIVATE cxx_std_11)

find_package(Vulkan REQUIRED)
# # Find GLSL validator for SPIR-V shader compilation (Included in LunarG SDK)
unset(GLSLValidator CACHE)
find_program(GLSLValidator "glslangValidator")
if(NOT GLSLValidator)
	message(WARNING "No GLSL validator found. SPIR-V shaders will not be compiled!")
endif()
find_package(glew REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(VkRenderer
	PUBLIC
		glm::glm
	PRIVATE
		Vulkan::Vulkan glew::glew
)

target_compile_definitions(VkRenderer PUBLIC
	# Whether or not to use Win32 as surface
	$<$<BOOL:${WIN32}>:USE_PLATFORM_WIN32>
)

target_compile_options(VoxelWorld PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall
		-Wextra
		-pedantic>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W3>
)

file(GLOB PublicHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB PrivateHeaders ${CMAKE_CURRENT_SOURCE_DIR}/source/*.h ${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp)
target_sources(VkRenderer PRIVATE ${PublicHeaders} ${PrivateHeaders})
# source_group(Headers\\public FILES ${PublicHeaders})
# source_group(Headers\\private FILES ${PrivateHeaders})


include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/VkRenderer)

install(TARGETS VkRenderer
	EXPORT VkRenderer-targets
	LIBRARY DESTINATION {CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT VkRenderer-targets
	FILE
		VkRendererTargets.cmake
	NAMESPACE
		Voxel::
	DESTINATION
		${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/VkRendererConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/VkRendererConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/VkRendererConfig.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT VkRenderer-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/VkRendererTargets.cmake NAMESPACE Voxel::)
export(PACKAGE VkRenderer)
