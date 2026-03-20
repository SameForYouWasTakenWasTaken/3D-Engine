#include <Components/Model.hpp>

#include "Engine/Systems/MaterialManager.hpp"
#include "Engine/Systems/Texture2DManager.hpp"

Model::Model(const std::string& path)
{
    loadModel(path);
}

const std::vector<SubMesh>& Model::GetSubMeshes() const
{
    return this->m_Meshes;
}

void Model::loadModel(const std::string& path)
{
    m_Meshes.clear();
    m_Directory.clear();
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        // TODO: Error logging
        // cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }

    // Windows may use back slashes for paths, this solves both normal and back slashes
    size_t slash = path.find_last_of("/\\");
    m_Directory = (slash == std::string::npos) ? "" : path.substr(0, slash);

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

SubMesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};

        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if (mesh->HasNormals())
        {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.tex_coords = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        if (mesh->HasVertexColors(0))
        {
            const aiColor4D& col = mesh->mColors[0][i];
            vertex.color = {col.r, col.g, col.b, col.a};
        }
        else
        {
            vertex.color = {1.f, 1.f, 1.f, 1.f};
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    auto materialID = static_cast<MaterialID>(-1);

    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        materialID = processMaterial(aiMat);
    }
    auto meshPtr = std::make_shared<Mesh>(std::move(vertices), std::move(indices));
    meshPtr->SetData();
    meshPtr->InModel = true;
    return SubMesh{meshPtr, materialID};
}

MaterialID Model::processMaterial(aiMaterial* aiMat)
{
    auto& services = Services::Get();
    auto& materialManager = services.GetService<MaterialManager>();
    auto& shaderManager = services.GetService<ShaderManager>();

    auto shader = shaderManager.Load("Shaders/first.vert", "Shaders/first.frag");
    auto diffuse = loadMaterialTexture(aiMat, aiTextureType_DIFFUSE);
    auto specular = loadMaterialTexture(aiMat, aiTextureType_SPECULAR);

    Material mat{};
    mat.shader = shader;
    mat.diffuse = diffuse;
    mat.specular = specular;
    // ambient, shininess and transparency already defined with default

    float transparency = mat.transparency;
    float shininess = mat.shininess;

    // Transparency
    if (aiMat->Get(AI_MATKEY_OPACITY, transparency) == AI_SUCCESS)
    {
        mat.transparency = transparency;
    }

    // Shininess
    if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
        mat.shininess = shininess;

    // Ambient color
    aiColor3D color(0.1f, 0.1f, 0.1f);
    if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
    {
        mat.ambient = {color.r, color.g, color.b};
    }

    return materialManager.Load(mat);
}

TextureID Model::loadMaterialTexture(aiMaterial* mat, aiTextureType type)
{
    if (mat->GetTextureCount(type) == 0)
        return static_cast<TextureID>(-1);

    aiString str;
    if (mat->GetTexture(type, 0, &str) != AI_SUCCESS)
        return static_cast<TextureID>(-1);

    std::string filename = str.C_Str();
    filename = m_Directory + "/" + filename;

    auto& textureManager = Services::Get().GetService<Texture2DManager>();

    auto texID = textureManager.Load(filename);
    if (!texID.has_value())
        return static_cast<TextureID>(-1);

    return texID.value();
}
