#pragma once

#include <Engine/Layer.hpp>

class PresentLayer : public Layer
{
     Renderer* renderer;
public:
    PresentLayer() {};
    ~PresentLayer() override = default;

    void OnDraw() override;
    void OnAttach() override;
    void OnUpdate(float dt) override {}
    void OnEvent(Event& e) override {}
    void OnDetach() override {}
};