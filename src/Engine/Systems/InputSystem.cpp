#include <Engine/Systems/InputSystem.hpp>

std::bitset<1024> InputSystem::m_Keys; // static
bool InputSystem::IsKeyHeld(int key)
{
    if (key < 0 || key >= m_Keys.size())
        return false;
    return m_Keys[key];
}

void InputSystem::OnEvent(Event& e)
{
    if (e.GetType() == KeyInputEvent::GetStaticType())
    {
        auto& key = static_cast<KeyInputEvent&>(e);
        if (key.Key < 0 || key.Key >= m_Keys.size())
            return; // ignore invalid key codes
        
        if (key.Action == GLFW_PRESS)
            m_Keys[key.Key] = true;
            
        if (key.Action == GLFW_RELEASE)
            m_Keys[key.Key] = false;
    }
}