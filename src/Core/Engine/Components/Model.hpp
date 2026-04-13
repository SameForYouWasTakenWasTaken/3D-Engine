#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Engine/Systems/Services.hpp"

#include "Texture.hpp"
#include "Engine/Components/Mesh.hpp"
#include "Engine/Components/Material.hpp"
#include "Engine/Renderer/API/Shader.hpp"

#include <vendor/entt.hpp>

using ModelID = uint64_t;

struct SubMesh
{
    MeshID mesh;
    MaterialID material;
};

class Model
{
    // model data
    std::vector<SubMesh> m_Meshes;
    std::string m_Directory;

    void loadModel(const std::string& path);
    void processNode(aiNode *node, const aiScene *scene);
    SubMesh processMesh(aiMesh *mesh, const aiScene *scene);
    MaterialID processMaterial(aiMaterial *material);
    TextureID loadMaterialTexture(aiMaterial *mat, aiTextureType type);

public:
    Model() = default;
    Model(const std::string& path);
    [[nodiscard]] const std::vector<SubMesh>& GetSubMeshes() const;
    [[nodiscard]] std::string GetDirectory() const;
    void ReloadModel(const std::string& path);
};

struct COMPModel
{
    ModelID id = static_cast<ModelID>(-1);
    std::vector<MaterialID> materialOverrides;
};
