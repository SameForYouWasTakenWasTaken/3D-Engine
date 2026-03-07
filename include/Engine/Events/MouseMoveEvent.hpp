#pragma once

#include <GLFW/glfw3.h>
#include <Engine/Events.hpp>

class MouseMoveEvent : public Event 
{
public:
    GLFWwindow* Window;
    double xpos, ypos;
    MouseMoveEvent(GLFWwindow* window, double x, double y) : Window(window), xpos(x), ypos(y) {}

    EVENT_TYPE(MouseMoveEvent);
};