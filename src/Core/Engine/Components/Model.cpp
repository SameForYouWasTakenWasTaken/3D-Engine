#include "Model.hpp"

#include "Engine/Systems/MaterialManager.hpp"
#include "Engine/Systems/MeshManager.hpp"
#include "Engine/Systems/Texture2DManager.hpp"
#include "tracy/Tracy.hpp"

/**
 * @brief Constructs a Model and loads its data from a file.
 *
 * @param path Filesystem path to the model file (absolute or relative). The constructor will load the model and populate internal meshes and material data from this file.
 */
Model::Model(const std::string& path)
{
    ReloadModel(path);
}

/**
 * Provides access to the model's loaded submeshes.
 *
 * @return const std::vector<SubMesh>& Const reference to the internal list of SubMesh objects owned by the model.
 */
const std::vector<SubMesh>& Model::GetSubMeshes() const
{
    return this->m_Meshes;
}

/**
 * @brief Retrieve the directory portion of the model's file path.
 *
 * @return std::string The directory path derived from the original model file path,
 *                     or an empty string if no directory component was present.
 */
std::string Model::GetDirectory() const
{
    return m_Directory;
}

void Model::ReloadModel(const std::string& path)
{
    loadModel(path);
}

/**
 * @brief Loads a 3D model file, converts it into submeshes, and populates the model state.
 *
 * Clears any existing mesh and directory state, attempts to read and validate the file using Assimp
 * (triangulating faces and generating smooth normals), sets the model directory to the file's
 * containing folder for relative texture resolution, and recursively processes the scene graph
 * to build submeshes.
 *
 * If the Assimp import fails or the scene is incomplete, the function returns early and leaves the
 * model in the cleared state (no submeshes).
 *
 * @param path Filesystem path to the model file to load.
 */
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

/**
 * @brief Traverses an Assimp node subtree and converts referenced meshes into SubMesh entries.
 *
 * Processes each mesh referenced by the given node and appends the resulting SubMesh objects
 * to the model's internal mesh list (`m_Meshes`), then recursively processes all child nodes.
 *
 * @param node Root Assimp node whose meshes and descendants will be processed.
 * @param scene Assimp scene that contains the mesh and node data referenced by `node`.
 */
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

/**
 * @brief Converts an Assimp mesh into an engine SubMesh.
 *
 * Builds vertex and index buffers from the provided `aiMesh`, resolves the mesh's
 * material using the containing `aiScene`, and returns a SubMesh that holds a
 * fully initialized `Mesh` and the associated `MaterialID`.
 *
 * @param mesh Assimp mesh to convert; must be non-null.
 * @param scene Assimp scene used to resolve the mesh's material index.
 * @return SubMesh A SubMesh containing the created `Mesh` and its `MaterialID`.
 *         Returns `MaterialID(-1)` when no valid material is found or assigned.
 */
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

    auto& meshManager = Services::Get().GetService<MeshManager>();
    auto FinalMesh = Mesh(std::move(vertices), std::move(indices));
    FinalMesh.SetData();
    FinalMesh.InModel = true;
    MeshID id = meshManager.Load(std::move(FinalMesh));
    return SubMesh{id, materialID};
}

/**
 * @brief Construct and register an engine Material from an Assimp material.
 *
 * Builds a Material by loading a default shader, resolving diffuse and specular textures,
 * and applying optional numeric and color overrides (opacity, shininess, ambient) from the
 * provided aiMaterial, then registers the material with the MaterialManager.
 *
 * @param aiMat Pointer to the Assimp material to convert.
 * @return MaterialID Identifier of the registered material.
 */
MaterialID Model::processMaterial(aiMaterial* aiMat)
{
    auto& services = Services::Get();
    auto& materialManager = services.GetService<MaterialManager>();
    auto& shaderManager = services.GetService<ShaderManager>();

    auto shader = shaderManager.Load(
        std::string(SHADERS_DIRECTORY) + "first.vert",
        std::string(SHADERS_DIRECTORY) + "first.frag"
    );
    auto diffuse = loadMaterialTexture(aiMat, aiTextureType_DIFFUSE);
    auto specular = loadMaterialTexture(aiMat, aiTextureType_SPECULAR);

    Material mat{};
    mat.shader = shader;
    mat.diffuse = diffuse;
    mat.specular = specular;
    // ambient, shininess and transparency already defined with default
    float transparency = mat.data.transparency;
    float shininess = mat.data.shininess;

    // Transparency
    if (aiMat->Get(AI_MATKEY_OPACITY, transparency) == AI_SUCCESS)
    {
        mat.data.transparency = transparency;
    }

    // Shininess
    if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
        mat.data.shininess = shininess;

    // Ambient color
    aiColor3D color(0.1f, 0.1f, 0.1f);
    if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
    {
        mat.data.ambient = {color.r, color.g, color.b};
    }
    auto matID = materialManager.Load(mat);
    return matID;
}

/**
 * @brief Loads the first texture of the given Assimp material for the specified texture type,
 *        resolving the texture path relative to the model's directory.
 *
 * @param mat Assimp material to query for textures.
 * @param type Assimp texture type to load (e.g., aiTextureType_DIFFUSE, aiTextureType_SPECULAR).
 * @return TextureID The identifier of the loaded texture, or `TextureID(-1)` if the material
 *         has no texture of that type, the texture entry cannot be retrieved, or loading fails.
 */
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
    TextureSettings settings{};

    if (type == aiTextureType_SPECULAR)
        settings.Texture_Use_sRGB = false;
    if (type == aiTextureType_NORMALS)
        settings.Texture_Use_sRGB = false;

    auto texID = textureManager.Load(filename, settings);
    if (!texID.has_value())
        return static_cast<TextureID>(-1);

    return texID.value();
}
