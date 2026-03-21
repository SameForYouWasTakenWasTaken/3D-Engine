#pragma once

#include <unordered_map>
#include <vector>

#include <App/Services.hpp>
#include <Engine/Systems/SceneManager.hpp>
#include <Engine/Systems/ShaderManager.hpp>
#include <Engine/Systems/MaterialManager.hpp>
#include <Engine/Systems/LightManager.hpp>
#include <Engine/Systems/Texture2DManager.hpp>
#include <Engine/LowLevel/Texture2D.hpp>
#include <Components/Mesh.hpp>
#include <Engine/Events.hpp>
#include <Contexts/EngineContext.hpp>

class SceneManager; // forward declaration
class EngineContext; // forward declaration
class Scene; // forward declaration

class Renderer : public IService {

    struct Batch
    {
        Mesh* mesh;
        Scene* active_scene;
        COMPTransform* transform;
        uint32_t materialID;
        
        std::vector<InstanceData> instances;
        GLuint instanceVBO = 0;
    };

    struct RenderObject
    {
        Mesh* mesh;
        Shader* shader;
        Material* material;
        std::shared_ptr<Texture2D> diffuse;
        std::shared_ptr<Texture2D> specular;
        COMPTransform* transform;
        Scene* scene; // The scene it is currently in
    };

    std::unordered_map<size_t, Batch> m_Batches;

public:
    EngineContext& m_EngineContext;
    Logger logger = Logger("RENDERER");

    Renderer(EngineContext& context);
    ~Renderer() = default;

    void Submit(Mesh* mesh, uint32_t materialID, COMPTransform* transform, Scene* active_scene);
    void CacheDrawObject(RenderObject& object);
    void CacheDrawObjectOutline(const RenderObject& object, float scaleAmount);

    void Begin();
    void Update(float dt);
    void End();
    
    void OnEvent(Event& e);
};