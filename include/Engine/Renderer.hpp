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
#include <Engine/LowLevel/FBO.hpp>

class SceneManager; // forward declaration
class EngineContext; // forward declaration
class Scene; // forward declaration

// Object data that gets submitted
struct SubmitObject
{
    Mesh* mesh;
    COMPTransform* transform;
    Scene* active_scene;
    uint32_t materialID;

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


    // GPU
    FBO m_SceneFBO;

    VAO m_ScreenVAO;
    VBO m_ScreenVBO;
    EBO m_ScreenEBO;

    ShaderID m_PreprocessShaderID;
    ShaderID m_WireframeShaderID;

public:
    RenderStats g_RenderStats; // Statistics for performance managing
    EngineContext& m_EngineContext;
    Logger logger = Logger("RENDERER");

    Renderer(EngineContext& context);
    /**
 * @brief Destroys the Renderer and releases owned resources.
 *
 * @details Uses the default destructor behavior; member objects and RAII-managed resources
 * are cleaned up by their own destructors.
 */
~Renderer() = default;

    void Submit(SubmitObject& submit);
    void PrepareObject(RenderObject& object);

    void DrawObject(RenderObject& object, size_t InstanceCount);
    void ApplyState();
    void RenderSceneToFBO();
    void PresentScene();
    void Init();
    void Begin();
    void Update(float dt);
    void End();

    void OnEvent(Event& e);
};