cmake_minimum_required(VERSION 3.15)
project(Game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif ()

add_subdirectory(src)

add_executable(GLmain src/main.cpp)
target_link_libraries(GLmain PUBLIC App)

 # enable_testing() # no need for testing now
 # add_subdirectory(tests)