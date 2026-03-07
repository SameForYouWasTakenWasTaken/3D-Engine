#pragma once
#include <glfw/glfw3.h>
#include <Engine/Events.hpp>

class KeyInputEvent : public Event
{
public:
    GLFWwindow* Window; 
    int Key; 
    int Scancode; 
    int Action; 
    int Mods;

    KeyInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
     : Window(window), Key(key), Scancode(scancode), Action(action), Mods(mods) {}

    bool IsKey(int key) { return Key == key; }
    bool IsKeyPressed() { return Action == GLFW_PRESS; }
    bool IsKeyReleased() { return Action == GLFW_RELEASE; }
    EVENT_TYPE(KeyInputEvent);
};