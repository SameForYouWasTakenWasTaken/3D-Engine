#pragma once

#include <GLFW/glfw3.h>
#include <Engine/Events.hpp>

class MouseMoveEvent : public Event 
{
public:
    GLFWwindow* window;
    double xpos, ypos;
    MouseMoveEvent(GLFWwindow* window, double x, double y) : xpos(x), ypos(y) {}

    EVENT_TYPE(MouseMoveEvent);
};