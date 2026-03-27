#pragma once

#include <bitset>
#include "Engine/Events/Events.hpp"
#include "Engine/Systems/Services.hpp"
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
 * @brief Disable copying of InputSystem instances.
 *
 * The copy constructor is deleted to prevent creating duplicate service instances (shared static
 * state such as key storage must not be copied).
 */
InputSystem(const InputSystem&) = delete;
        /**
     * @brief Default destructor for InputSystem.
     */
    ~InputSystem() = default;

    static bool IsKeyHeld(int key);

    static void OnEvent(const KeyInputEvent& key);
};