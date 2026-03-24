#pragma once

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>
#include <entt.hpp>
#include <glm/mat4x4.hpp>
#include <Engine/Renderer.hpp>
#include <Components/Camera.hpp>

#include <Logger.hpp>

enum CULLING_MODE
{
    NONE, // Null
    FRONT = GL_FRONT,
    BACK = GL_BACK,
    FRONT_AND_BACK = GL_FRONT_AND_BACK
};

enum class DEPTH_FUNC
{
    NONE, // Null
    NEVER = GL_NEVER,
    LESS = GL_LESS,
    EQUAL = GL_EQUAL,
    LEQUAL = GL_LEQUAL,
    GREATER = GL_GREATER,
    NOTEQUAL = GL_NOTEQUAL,
    GEQUAL = GL_GEQUAL,
    ALWAYS = GL_ALWAYS
};

enum class DEPTH_WRITE
{
    NONE, // null
    ENABLED,
    DISABLED
};

enum class WINDING_DIR
{
    NONE, // Null
    CLOCKWISE = GL_CW,
    COUNTER_CLOCKWISE = GL_CCW,
};

enum class OPTION
{
    NONE, // Null
    YES,
    NO
};

struct GLStateCache
{
    OPTION VSync = OPTION::YES;
    OPTION Wireframe = OPTION::NO;

    OPTION DepthTest = OPTION::YES;
    DEPTH_FUNC DepthFunc = DEPTH_FUNC::LESS;
    DEPTH_WRITE DepthWrite = DEPTH_WRITE::ENABLED;

    OPTION StencilTest = OPTION::YES;
    OPTION ColorBlend = OPTION::YES;

    OPTION Culling = OPTION::YES;
    CULLING_MODE CullingMode = CULLING_MODE::BACK;
    WINDING_DIR WindingDir = WINDING_DIR::COUNTER_CLOCKWISE;
};

/**
 * @brief Convert a CULLING_MODE value to the corresponding OpenGL cull-face `GLenum`.
 *
 * @param mode Culling mode to convert.
 * @return GLenum OpenGL constant representing the cull-face mode for `mode`; returns `GL_BACK` if `mode` is unrecognized.
 */
inline GLenum ToGLCullMode(CULLING_MODE mode)
{
    switch (mode)
    {
    case CULLING_MODE::BACK: return GL_BACK;
    case CULLING_MODE::FRONT: return GL_FRONT;
    case CULLING_MODE::FRONT_AND_BACK: return GL_FRONT_AND_BACK;
    }
    return GL_BACK;
}

/**
 * @brief Convert a WINDING_DIR value to the corresponding OpenGL winding direction.
 *
 * @param dir The WINDING_DIR enum value to convert.
 * @return GLenum `GL_CW` for `WINDING_DIR::CLOCKWISE`, `GL_CCW` for `WINDING_DIR::COUNTER_CLOCKWISE`; defaults to `GL_CCW`.
 */
inline GLenum ToGLCullDir(WINDING_DIR dir)
{
    switch (dir)
    {
    case WINDING_DIR::CLOCKWISE: return GL_CW;
    case WINDING_DIR::COUNTER_CLOCKWISE: return GL_CCW;
    }
    return GL_CCW;
}

class Renderer; // Forward declaration
struct EngineContext
{
    // Window properties
    GLFWwindow* ActiveWindow = nullptr;
    int WindowWidth, WindowHeight;

    // Boot
    bool SafeShutdown = false; // If a critical error happens, this should be toggled to safely shutdown

    // Events
    entt::dispatcher EventDispatcher;

    // Rendering
    GLStateCache StateCache;

    // Logging related stuff
    Logger logger = Logger("EngineContext");
};
