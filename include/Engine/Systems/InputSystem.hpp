#pragma once

#include <bitset>
#include <Engine/Events.hpp>
#include <Engine/Events/KeyInputEvent.hpp>

class InputSystem
{
    static std::bitset<1024> m_Keys;
public:
    InputSystem() = default;
    ~InputSystem() = default;

    static bool IsKeyHeld(int key);

    static void OnEvent(Event& e);
};