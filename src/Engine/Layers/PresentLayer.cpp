#include <Engine/Layers/PresentLayer.hpp>

void PresentLayer::OnDraw()
{
    renderer->End();
}

void PresentLayer::OnAttach()
{
    renderer = &Services::Get().GetService<Renderer>();
}
