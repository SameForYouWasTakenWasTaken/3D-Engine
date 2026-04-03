#pragma once

#include <GLFW/glfw3.h>
#include "Events.hpp"

class WindowResizeEvent : public Event
{
public:
    GLFWwindow* Window;
    int Width, Height;

    WindowResizeEvent(GLFWwindow* window, int width, int height) : Window(window), Width(width), Height(height) {}
    EVENT_TYPE(WindowResizeEvent);
};