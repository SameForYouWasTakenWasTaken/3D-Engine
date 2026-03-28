#pragma once

#include <GLFW/glfw3.h>
#include "Events.hpp"

class WindowResizeEvent : public Event
{
public:
    GLFWwindow* Window;
    double Width, Height;

    WindowResizeEvent(GLFWwindow* window, double width, double height) : Window(window), Width(width), Height(height) {}
    EVENT_TYPE(WindowResizeEvent);
};