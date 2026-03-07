#pragma once
#include <GLFW/glfw3.h>
#include <Engine/Events.hpp>

class KeyInputEvent : public Event
{
public:
    GLFWwindow* Window; 
    int Key; 
    int Scancode; 
    int Action; 
    int Mods;

    /**
      * @brief Constructs a KeyInputEvent linking a GLFW window with its key event data.
      *
      * @param window Pointer to the GLFW window where the event originated.
      * @param key The platform-independent key code for the key involved in the event.
      * @param scancode The platform-specific scancode for the physical key.
      * @param action The action for the event (e.g., GLFW_PRESS, GLFW_RELEASE).
      * @param mods Bitfield of modifier keys active during the event (e.g., GLFW_MOD_SHIFT).
      */
     KeyInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
     : Window(window), Key(key), Scancode(scancode), Action(action), Mods(mods) {}

    /**
 * @brief Checks whether the event's key matches the given key code.
 *
 * @param key Key code to compare against the event's key.
 * @return `true` if the event's key equals `key`, `false` otherwise.
 */
bool IsKey(int key) { return Key == key; }
    /**
 * @brief Checks whether the event represents a key press.
 *
 * @return `true` if the event's action is GLFW_PRESS, `false` otherwise.
 */
bool IsKeyPressed() { return Action == GLFW_PRESS; }
    /**
 * @brief Checks whether the key event represents a key release.
 *
 * @return `true` if the event's action equals `GLFW_RELEASE`, `false` otherwise.
 */
bool IsKeyReleased() { return Action == GLFW_RELEASE; }
    EVENT_TYPE(KeyInputEvent);
};