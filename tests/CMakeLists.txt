include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(engine_tests
        test_light.cpp
        test_material.cpp
        test_vertex.cpp
        test_light_manager.cpp
        test_texture2d_manager.cpp
        test_shader_parse.cpp
        test_transform.cpp
        test_camera.cpp
        test_services.cpp
        test_light_types.cpp
        # Source files needed for testing
        ${CMAKE_SOURCE_DIR}/src/Engine/Systems/LightManager.cpp
        ${CMAKE_SOURCE_DIR}/src/Engine/Systems/Texture2DManager.cpp
        ${CMAKE_SOURCE_DIR}/src/Engine/LowLevel/Shader.cpp
        ${CMAKE_SOURCE_DIR}/src/Engine/LowLevel/Texture2D.cpp
        ${CMAKE_SOURCE_DIR}/src/Logger.cpp
        ${CMAKE_SOURCE_DIR}/src/glad.c
        ${CMAKE_SOURCE_DIR}/src/Engine/LowLevel/stb_image.c
        ${CMAKE_SOURCE_DIR}/src/Components/Transform.cpp
        ${CMAKE_SOURCE_DIR}/src/Components/Camera.cpp
        ${CMAKE_SOURCE_DIR}/src/Components/Light.cpp
        ${CMAKE_SOURCE_DIR}/src/App/Services.cpp
)

# Include directories for tests
target_include_directories(engine_tests PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
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