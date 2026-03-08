#pragma once

#include <unordered_map>
#include <vector>
#include <ranges>

#include <entt.hpp>

#include <Engine/Systems/SceneManager.hpp>
#include <Engine/Systems/ShaderManager.hpp>
#include <Engine/Systems/MaterialManager.hpp>
#include <Engine/Systems/Texture2DManager.hpp>
#include <Engine/LowLevel/VAO.hpp>
#include <Engine/LowLevel/VBO.hpp>
#include <Engine/LowLevel/EBO.hpp>
#include <Engine/LowLevel/Texture2D.hpp>
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
        uint32_t materialID;
        
        std::vector<InstanceData> instances;

        GLuint instanceVBO = 0;
    };

    std::unordered_map<size_t, Batch> m_Batches;

public:
    ShaderManager m_ShaderManager;
    MaterialManager m_MaterialManager = MaterialManager(m_ShaderManager);
    Texture2DManager m_TextureManager;
    
    EngineContext& m_EngineContext;
    Logger logger = Logger("RENDERER");

    Renderer(EngineContext& context);
    ~Renderer() = default;

    
    void Submit(Mesh* mesh, uint32_t materialID, const glm::mat4& model);
    
    void Begin();
    void Update(float dt);
    void End();
    
    void OnEvent(Event& e);
};