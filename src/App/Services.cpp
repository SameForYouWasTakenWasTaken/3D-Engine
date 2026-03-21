#include <App/Services.hpp>

/**
 * @brief Provides access to the single, shared Services instance.
 *
 * Returns the application's singleton Services instance for global access.
 *
 * @return Services& Reference to the single shared Services instance.
 */
Services& Services::Get()
{
    static Services instance;
    return instance;
}

