
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_library(Engine ${ENGINE_SRC})
target_include_directories(Engine PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../vendor"
)


target_link_libraries(Engine
        PUBLIC
        glfw
        glm::glm
        spdlog::spdlog
        assimp::assimp
        Tracy::TracyClient
)