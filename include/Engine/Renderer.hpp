#pragma once

#include <unordered_map>
#include <vector>
#include <ranges>

#include <entt.hpp>

#include <Engine/Systems/SceneManager.hpp>
#include <Engine/LowLevel/VAO.hpp>
#include <Engine/LowLevel/VBO.hpp>
#include <Engine/LowLevel/EBO.hpp>
#include <Engine/LowLevel/Shader.hpp>
#include <Components/Drawable.hpp>
#include <Components/Transform.hpp>
#include <Components/Material.hpp>
#include <Components/Mesh.hpp>
#include <Engine/Layer.hpp>
#include <Engine/Events.hpp>
#include <Contexts/EngineContext.hpp>
#include <Tags.hpp>

class SceneManager; // forward declaration
class EngineContext; // forward declaration

class Renderer final {
    struct InstanceData
    {
        glm::mat4 model;
    };

    struct Batch
    {
        Mesh* mesh;
        COMPMaterial* material;
        
        std::vector<InstanceData> instances;

        GLuint instanceVBO = 0;
    };

    std::unordered_map<size_t, Batch> m_Batches;
public:
    EngineContext& m_EngineContext;
    std::unique_ptr<SceneManager> m_SceneManager;
    Logger logger = Logger("RENDERER");

    Renderer(EngineContext& context);
    ~Renderer() = default;

    
    void Submit(Mesh* mesh, COMPMaterial* material, const glm::mat4& model);
    
    void Begin();
    void Update(float dt);
    void End();
    
    void OnEvent(Event& e);
};