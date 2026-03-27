#include "InputSystem.hpp"

std::bitset<1024> InputSystem::m_Keys; /**
 * @brief Determines whether the specified key is currently held down.
 *
 * @param key Key code to query; valid range is 0 through m_Keys.size() - 1 (0..1023).
 * @return bool `true` if the key is marked as pressed, `false` if it is not pressed or the key code is out of range.
 */
bool InputSystem::IsKeyHeld(int key)
{
    if (key < 0 || key >= m_Keys.size())
        return false;
    return m_Keys[key];
}

/**
 * @brief Handles input events to update the global key state table.
 *
 * Updates the static key-state bitset for key press and release events; ignores events of other types and invalid key codes.
 *
 * @param e Event to process; when it is a KeyInputEvent with a valid key code, the corresponding entry in the key state bitset is set on press and cleared on release.
 */
void InputSystem::OnEvent(const KeyInputEvent& key)
{
    if (key.Key < 0 || key.Key >= m_Keys.size())
        return; // ignore invalid key codes

    if (key.Action == GLFW_PRESS)
        m_Keys[key.Key] = true;

    if (key.Action == GLFW_RELEASE)
        m_Keys[key.Key] = false;
}