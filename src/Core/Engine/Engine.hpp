#pragma once
#include "EngineSettings.hpp"
#include "Contexts/EngineContext.hpp"

class Engine final
{
    static EngineContext m_EngineContext;
public:
    Engine() = default;
    static bool Init(EngineSettings settings);
    static void InitServices();
    static EngineContext& GetContext()
    {
        return m_EngineContext;
    }
};
