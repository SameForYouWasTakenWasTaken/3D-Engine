add_library(SharedLib INTERFACE)

target_include_directories(SharedLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SharedLib INTERFACE glm::glm)
