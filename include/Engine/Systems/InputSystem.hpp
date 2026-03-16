#pragma once

#include <bitset>
#include <Engine/Events.hpp>
#include <App/Services.hpp>
#include <Engine/Events/KeyInputEvent.hpp>

class InputSystem : public IService
{
    static std::bitset<1024> m_Keys;
public:
        /**
     * @brief Constructs a new InputSystem instance.
     *
     * Initializes an InputSystem object; all state is default-initialized.
     */
    InputSystem() = default;
        /**
     * @brief Default destructor for InputSystem.
     */
    ~InputSystem() = default;

    static bool IsKeyHeld(int key);

    static void OnEvent(Event& e);
};