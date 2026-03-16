cmake_minimum_required(VERSION 3.15)
project(Game)

# Project config
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

# Collect sources and headers
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")

# If CMAKE_BUILD_TYPE is not set, set it to Debug
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

# GLFW
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.4
)
# GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG master
)
# spdlog for logging
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG v1.15.3
)
# Google Test
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v1.15.2
)
# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw glm spdlog googletest)

set(PROJECT_INCLUDES
	"${CMAKE_SOURCE_DIR}/include"
)

set(REQUIRED_LIBRARIES glm::glm spdlog::spdlog glfw)

add_executable(main ${SRC_FILES} ${HEADER_FILES})

# Include directories

target_include_directories(main PRIVATE ${PROJECT_INCLUDES})

# Link libraries
target_link_libraries(main PRIVATE ${REQUIRED_LIBRARIES} stdc++exp)

# Copy shaders after build
add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/Shaders"
	$<TARGET_FILE_DIR:main>/Shaders
)

add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/Resources"
	$<TARGET_FILE_DIR:main>/Resources
)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)