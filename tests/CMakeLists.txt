# Test executable
add_executable(engine_tests
    test_light.cpp
    test_material.cpp
    test_vertex.cpp
    test_light_manager.cpp
    test_texture2d_manager.cpp
    test_shader_parse.cpp
    # Source files needed for testing
    ${CMAKE_SOURCE_DIR}/src/Engine/Systems/LightManager.cpp
    ${CMAKE_SOURCE_DIR}/src/Engine/Systems/Texture2DManager.cpp
    ${CMAKE_SOURCE_DIR}/src/Engine/LowLevel/Shader.cpp
)

# Include directories for tests
target_include_directories(engine_tests PRIVATE
    ${PROJECT_INCLUDES}
)

# Link against Google Test and project dependencies
target_link_libraries(engine_tests PRIVATE
    GTest::gtest_main
    glm::glm
    spdlog::spdlog
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(engine_tests)