#pragma once

#include <optional>
#include <memory>
#include "vendor/Logger.hpp"

#include "Layer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Contexts/SceneContext.hpp"

class Layer;
class Scene final
{
    uint32_t SceneID = -1;
    SceneContext m_SceneContext{};
    std::vector<std::shared_ptr<Layer>> m_Layers{};
public:

    Logger logger = Logger("SCENE");
    /**
 * @brief Constructs a Scene with default-initialized members.
 *
 * Initializes an empty scene: no layers, a default CameraManager, a fresh entity registry,
 * and other members set to their declared default values.
 */
    Scene();
    /**
 * @brief Default destructor for Scene.
 *
 * Performs any required cleanup when a Scene instance is destroyed.
 */
    ~Scene() = default;

    void Update(float dt);
    template <typename T, typename... Args> requires(std::derived_from<T, Layer>)
    std::shared_ptr<T> PushLayer(Args&& ... args);

    template <typename T> requires(std::derived_from<T, Layer>)
    void RemoveLayer(std::shared_ptr<T> layer);

    void OnAttach(uint32_t id); // On attach to the scene manager
    void OnDraw();
    void OnUpdate(float dt);

    // EngineContext* because std::expected cant take reference
    SceneContext& GetContext();
    uint32_t GetID() const;
};

template <typename T, typename ... Args> requires (std::derived_from<T, Layer>)
std::shared_ptr<T> Scene::PushLayer(Args&&... args)
{
    auto layer = std::make_shared<T>(std::forward<Args>(args)...);
    m_Layers.push_back(layer);
    layer->SetScene(this);
    layer->OnAttach();
    return layer;
}

template <typename T> requires (std::derived_from<T, Layer>)
void Scene::RemoveLayer(std::shared_ptr<T> layer)
{
    if (!layer) return;
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);

    if (it == m_Layers.end()) return;
    layer->OnDetach();
    layer->SetScene(nullptr);
    m_Layers.erase(it);
}
