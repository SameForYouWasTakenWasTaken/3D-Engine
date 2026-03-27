#pragma once
#include <memory>
#include <optional>

#include "Texture.hpp"
#include "Engine/Components/Material.hpp"
#include "../Renderer/API/OpenGL/VAO.hpp"
#include "../Renderer/API/OpenGL/VBO.hpp"
#include "../Renderer/API/OpenGL/EBO.hpp"
#include "Engine/Systems/Services.hpp"

using MeshID = uint64_t;

struct InstanceData
{
    glm::mat4 model;

    // normals
    glm::vec4 normal0;
    glm::vec4 normal1;
    glm::vec4 normal2;
};

/**
 * @brief Provides the attribute description for a vertex position.
 *
 * Returns an AttributePointer configured for the position attribute used by the vertex shader: attribute location 0 with 3 components and the byte offset of Vertex::position.
 *
 * @return AttributePointer Configured for position (index = 0, size = 3, offset = offsetof(Vertex, position)).
 */
inline AttributePointer GetAttribPointerPos()
{
    AttributePointer pos;
    pos.index = 0;
    pos.size = 3;
    pos.offset = reinterpret_cast<void*>(offsetof(Vertex, position));
    return pos;
}

/**
 * @brief Provides the vertex attribute description for the vertex color attribute.
 *
 * @return AttributePointer Attribute description configured for attribute location 1 with 4 components, offset to `Vertex::color`.
 */
inline AttributePointer GetAttribPointerCol()
{
    AttributePointer col;
    col.index = 1;
    col.size = 4;
    col.offset = reinterpret_cast<void*>(offsetof(Vertex, color));
    return col;
}

/**
 * @brief Provides the attribute description for a vertex normal.
 *
 * @return AttributePointer configured with attribute location 3, 3 components, and the byte offset of `Vertex::normal`.
 */
inline AttributePointer GetAttribPointeraNormal()
{
    AttributePointer normal;
    normal.index = 3;
    normal.size = 3;
    normal.offset = reinterpret_cast<void*>(offsetof(Vertex, normal));
    return normal;
}

/**
 * @brief Provides the vertex attribute description for texture coordinates.
 *
 * @return AttributePointer Configured for attribute location 2 with two components, using the offset of `Vertex::tex_coords`.
 */
inline AttributePointer GetAttribPointerTex()
{
    AttributePointer tex;
    tex.index = 2;
    tex.size = 2;
    tex.offset = reinterpret_cast<void*>(offsetof(Vertex, tex_coords));
    return tex;
}

/**
 * @brief Describes the instanced normal0 vertex attribute for shader input.
 *
 * @return AttributePointer configured for the instance `normal0` attribute:
 * attribute location 8, 4 components (vec4), and byte offset to `InstanceData::normal0`.
 */
inline AttributePointer GetAttribPointerNormal0()
{
    AttributePointer attr{};
    attr.index = 8; // matches shader layout location
    attr.size = 4;  // vec4
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal0));
    return attr;
}

/**
 * @brief Provides the attribute description for the instanced normal1 vector.
 *
 * Returns an AttributePointer configured for shader location 9 that maps to InstanceData::normal1.
 *
 * @return AttributePointer with attribute index 9, component count 4, and offset pointing to `InstanceData::normal1`.
 */
inline AttributePointer GetAttribPointerNormal1()
{
    AttributePointer attr{};
    attr.index = 9;
    attr.size = 4;
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal1));
    return attr;
}

/**
 * @brief Provides the attribute description for the per-instance normal2 vector.
 *
 * Configures an AttributePointer for the instance attribute located at shader layout index 10,
 * with 4 components and the byte offset of InstanceData::normal2.
 *
 * @return AttributePointer Attribute description (index = 10, size = 4, offset = offsetof(InstanceData, normal2)).
 */
inline AttributePointer GetAttribPointerNormal2()
{
    AttributePointer attr{};
    attr.index = 10;
    attr.size = 4;
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal2));
    return attr;
}

constexpr size_t MAX_INSTANCES = 10000;
class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    
    uint32_t IndexCount;
    uint32_t VertexCount;

    VAO vao;
    VBO vbo;
    EBO ebo;
    VBO instanceVBO; // For instanced rendering


    bool Indexed = false;
    bool InModel = false; // Is this mesh apart of a model?

    GLenum Primitive = GL_TRIANGLES;

    void SetData(GLenum draw_type = GL_STATIC_DRAW);

    /**
     * @brief Constructs a Mesh by taking ownership of provided vertex and index arrays.
     *
     * The provided vectors are moved into the mesh's internal storage; index and vertex
     * counters are initialized to zero.
     *
     * @param vertices Vector of vertex data to transfer into the mesh (moved).
     * @param indices Vector of indices to transfer into the mesh (moved).
     */
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices)
        :
    vertices(std::move(vertices)), indices(std::move(indices)),
    IndexCount(0), VertexCount(0)
    {
    } 
};

struct COMPMesh
{
    MeshID mesh = static_cast<MaterialID>(-1);
    MaterialID materialOverride = static_cast<MaterialID>(-1);
};