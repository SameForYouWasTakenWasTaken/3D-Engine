#pragma once

#include <unordered_map>

#include "Engine/Systems/Services.hpp"
#include "Engine/Components/Mesh.hpp"

class MeshManager : public IService
{
    std::unordered_map<MeshID, Mesh> m_Meshes;
public:
    MeshManager() = default;

    MeshManager(const MeshManager&) = delete;

    [[nodiscard]]
    MeshID Load(Mesh&& mesh);

    [[nodiscard]]
    Mesh* Get(MeshID id);
};
