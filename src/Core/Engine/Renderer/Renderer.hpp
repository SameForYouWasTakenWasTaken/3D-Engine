#pragma once

#include <unordered_map>
#include <vector>

#include "Engine/Systems/Services.hpp"
#include <Engine/Systems/ShaderManager.hpp>
#include "Engine/Systems/MaterialManager.hpp"
#include "Engine/Renderer/API/Texture2D.hpp"
#include "Engine/Components/Mesh.hpp"
#include "Engine/Events/Events.hpp"
#include "Contexts/EngineContext.hpp"
#include "Contexts/SceneContext.hpp"
#include "Engine/Components/Transform.hpp"
#include "API/OpenGL/FBO.hpp"
#include "Engine/Events/WindowResizeEvent.hpp"

class EngineContext; // forward declaration
class Scene; // forward declaration

// Object data that gets submitted
struct SubmitObject
{
    MeshID meshID;
    COMPTransform* transform;
    SceneContext* context;
    MaterialID materialID;

    // Data overrides
    std::optional<MaterialOverride> material_override;
};

struct RenderStats
{
    uint64_t drawCalls = 0;
    uint64_t verticesSubmitted = 0;
    uint64_t indicesSubmitted = 0;
    uint64_t trianglesSubmitted = 0;

    /**
     * @brief Resets all render statistics counters to zero.
     *
     * Sets `drawCalls`, `verticesSubmitted`, `indicesSubmitted`, and `trianglesSubmitted` to 0.
     */
    void Reset()
    {
        drawCalls = 0;
        verticesSubmitted = 0;
        indicesSubmitted = 0;
        trianglesSubmitted = 0;
    }
};

class Renderer : public IService {

    struct Batch
    {
        MeshID meshID;
        SceneContext* context;
        COMPTransform* transform;
        MaterialID materialID;
        
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
        SceneContext* context; // The scene it is currently in
    };

    std::unordered_map<size_t, Batch> m_Batches;


    // GPU
    FBO m_SceneFBO;

    VAO m_ScreenVAO;
    VBO m_ScreenVBO;
    EBO m_ScreenEBO;

    ShaderID m_PreprocessShaderID;
    ShaderID m_WireframeShaderID;

public:
    RenderStats g_RenderStats; // Statistics for performance managing
    Logger logger = Logger("RENDERER");

    Renderer();
    /**
 * @brief Destroys the Renderer and releases owned resources.
 *
 * @details Uses the default destructor behavior; member objects and RAII-managed resources
 * are cleaned up by their own destructors.
 */
~Renderer() = default;

    void Submit(SubmitObject& submit);
    bool PrepareObject(RenderObject& object);

    void DrawObject(RenderObject& object, size_t InstanceCount);
    void ApplyState();
    void RenderSceneToFBO();
    void PresentScene();
    void Init();
    void Begin();
    void Update(float dt);
    void End();

    void OnResize(const WindowResizeEvent& resize);
};