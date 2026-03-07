#pragma once

#include <GLFW/glfw3.h>
#include <Engine/Events.hpp>

class MouseMoveEvent : public Event 
{
public:
    GLFWwindow* window;
    double xpos, ypos;
    /**
 * @brief Create a MouseMoveEvent with the specified window pointer and coordinates.
 *
 * Initializes `xpos` and `ypos` from `x` and `y`, respectively. The `window` member is not initialized by this constructor and may remain indeterminate.
 *
 * @param window Pointer to the GLFW window associated with the event (not stored by this constructor).
 * @param x Horizontal mouse position.
 * @param y Vertical mouse position.
 */
MouseMoveEvent(GLFWwindow* window, double x, double y) : xpos(x), ypos(y) {}

    EVENT_TYPE(MouseMoveEvent);
};