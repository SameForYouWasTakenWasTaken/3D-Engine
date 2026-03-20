#pragma once
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <App/Services.hpp>

#include <Components/Texture.hpp>
#include <Components/Mesh.hpp>
#include <Components/Material.hpp>
#include <Engine/LowLevel/Shader.hpp>

using ModelID = uint32_t;

struct SubMesh
{
    std::shared_ptr<Mesh> mesh;
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
    Model(const std::string& path);
    [[nodiscard]] const std::vector<SubMesh>& GetSubMeshes() const;
};

struct COMPModel
{
    ModelID id = static_cast<ModelID>(-1);
};
