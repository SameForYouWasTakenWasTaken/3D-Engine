#include <App/Services.hpp>

/**
 * @brief Accessor for the process-wide Services singleton.
 *
 * Returns a reference to the single Services instance; the instance is constructed on first call.
 * Initialization is thread-safe per C++11 and later.
 *
 * @return Services& Reference to the singleton Services instance.
 */
Services& Services::Get()
{
    static Services instance;
    return instance;
}

