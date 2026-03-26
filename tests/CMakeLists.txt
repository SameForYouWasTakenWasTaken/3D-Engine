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
)

# Link against Google Test and project dependencies
target_link_libraries(engine_tests PRIVATE
        Engine
        GTest::gtest_main
        glm::glm
        spdlog::spdlog
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(engine_tests)