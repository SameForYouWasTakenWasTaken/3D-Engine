#include "MeshManager.hpp"

inline MeshID HashMesh(const Mesh& mesh)
{
    std::size_t seed = 0;

    // Hash vertices
    for (const auto& v : mesh.vertices)
    {
        HashCombine(seed, v.position.x);
        HashCombine(seed, v.position.y);
        HashCombine(seed, v.position.z);

        HashCombine(seed, v.normal.x);
        HashCombine(seed, v.normal.y);
        HashCombine(seed, v.normal.z);

        HashCombine(seed, v.tex_coords.x);
        HashCombine(seed, v.tex_coords.y);
    }

    // Hash indices
    for (auto i : mesh.indices)
    {
        HashCombine(seed, i);
    }

    return static_cast<MeshID>(seed);
}

MeshID MeshManager::Load(Mesh&& mesh)
{
    auto id = HashMesh(mesh);
    if (Get(id) != nullptr)
        return id;
    m_Meshes.emplace(id, std::move(mesh));
    return id;
}

Mesh* MeshManager::Get(MeshID id)
{
    auto it = m_Meshes.find(id);
    if (it == m_Meshes.end())
        return nullptr;
    return &it->second;
}