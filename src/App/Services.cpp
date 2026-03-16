#include <App/Services.hpp>

Services& Services::Get()
{
    static Services instance;
    return instance;
}

