file(GLOB_RECURSE APP_FILES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_library(App
        ${APP_FILES}
)
target_link_libraries(App PUBLIC Engine imgui SharedLib)
target_include_directories(App PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


# Copy assets after build

add_custom_command(TARGET App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:App>/assets
)

set(ASSET_DIR "$<TARGET_FILE_DIR:App>/assets")
set(OBJECTS_DIR "${ASSET_DIR}/Objects")
set(SHADERS_DIR "${ASSET_DIR}/Shaders")
set(TEXTURES_DIR "${ASSET_DIR}/Textures2D")

# Give engine and app same directories
target_compile_definitions(Engine PRIVATE ASSET_DIRECTORY="${ASSET_DIR}/")
target_compile_definitions(Engine PRIVATE OBJECTS_DIRECTORY="${OBJECTS_DIR}/")
target_compile_definitions(Engine PRIVATE TEXTURES_DIRECTORY="${TEXTURES_DIR}/")
target_compile_definitions(Engine PRIVATE SHADERS_DIRECTORY="${SHADERS_DIR}/")

target_compile_definitions(App PRIVATE ASSET_DIRECTORY="${ASSET_DIR}/")
target_compile_definitions(App PRIVATE OBJECTS_DIRECTORY="${OBJECTS_DIR}/")
target_compile_definitions(App PRIVATE TEXTURES_DIRECTORY="${TEXTURES_DIR}/")
target_compile_definitions(App PRIVATE SHADERS_DIRECTORY="${SHADERS_DIR}/")
