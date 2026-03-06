#pragma once

#include <Engine/Events.hpp>

class WindowResizeEvent : public Event
{
public:
    int Width, Height;

    WindowResizeEvent(int width, int height) : Width(width), Height(height) {}
    EVENT_TYPE(WindowResizeEvent);
};