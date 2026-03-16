#pragma once
#include <memory>

#include <Components/Texture.hpp>
#include <Engine/LowLevel/VAO.hpp>
#include <Engine/LowLevel/VBO.hpp>
#include <Engine/LowLevel/EBO.hpp>

struct InstanceData
{
    glm::mat4 model;

    // normals
    glm::vec4 normal0;
    glm::vec4 normal1;
    glm::vec4 normal2;
};

/**
 * @brief Provides the attribute pointer configuration for vertex position data.
 *
 * @return AttributePointer Configured for the position attribute at shader location 0 with 3 components and an offset corresponding to `Vertex::position`.
 */
inline const AttributePointer GetAttribPointerPos()
{
    AttributePointer pos;
    pos.index = 0;
    pos.size = 3;
    pos.offset = reinterpret_cast<void*>(offsetof(Vertex, position));
    return pos;
}

/**
 * @brief Describes the vertex color attribute layout for shaders.
 *
 * @return AttributePointer configured for vertex color with index 1, size 4, and offset pointing to `Vertex::color`.
 */
inline const AttributePointer GetAttribPointerCol()
{
    AttributePointer col;
    col.index = 1;
    col.size = 4;
    col.offset = reinterpret_cast<void*>(offsetof(Vertex, color));
    return col;
}

/**
 * @brief Describes the vertex normal attribute layout in the Vertex struct for shader attribute location 3.
 *
 * @return AttributePointer Configuration with index = 3, size = 3, and offset = offsetof(Vertex, normal).
 */
inline const AttributePointer GetAttribPointeraNormal()
{
    AttributePointer normal;
    normal.index = 3;
    normal.size = 3;
    normal.offset = reinterpret_cast<void*>(offsetof(Vertex, normal));
    return normal;
}

/**
 * @brief Describes the vertex attribute layout for texture coordinates.
 *
 * @return AttributePointer configured for texture coordinates: attribute location 2, component count 2, and offset corresponding to Vertex::tex_coords.
 */
inline const AttributePointer GetAttribPointerTex()
{
    AttributePointer tex;
    tex.index = 2;
    tex.size = 2;
    tex.offset = reinterpret_cast<void*>(offsetof(Vertex, tex_coords));
    return tex;
}

/**
 * @brief Provides the vertex attribute description for the first per-instance normal (normal0).
 *
 * @return AttributePointer configured with index = 8, size = 3, and offset pointing to InstanceData::normal0.
 */
inline const AttributePointer GetAttribPointerNormal0()
{
    AttributePointer attr{};
    attr.index = 8; // matches shader layout location
    attr.size = 3;  // vec3
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal0));
    return attr;
}

/**
 * @brief Provides an attribute description for the second per-instance normal (normal1) used by instanced rendering.
 *
 * @return AttributePointer configured for attribute location 9, with size 3 (vec3) and offset set to InstanceData::normal1.
 */
inline const AttributePointer GetAttribPointerNormal1()
{
    AttributePointer attr{};
    attr.index = 9;
    attr.size = 3;
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal1));
    return attr;
}

/**
 * @brief Provides the attribute pointer for the third per-instance normal used in instanced rendering.
 *
 * @return AttributePointer configured for InstanceData.normal2: index = 10, size = 3, offset to InstanceData::normal2.
 */
inline const AttributePointer GetAttribPointerNormal2()
{
    AttributePointer attr{};
    attr.index = 10;
    attr.size = 3;
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal2));
    return attr;
}


class Mesh
{
public:
    std::vector<Vertex>& vertices;
    std::vector<GLuint>& indices;

    VAO vao;
    VBO vbo;
    EBO ebo;
    VBO instanceVBO; // For instanced rendering

    uint32_t IndexCount;
    uint32_t VertexCount;

    bool Indexed = false;
    GLenum Primitive = GL_TRIANGLES;

    void SetData(GLenum draw_type = GL_STATIC_DRAW);
    /**
         * @brief Initialize the mesh by storing references to external vertex and index arrays.
         *
         * The mesh stores references to the provided containers rather than copying them;
         * the caller is responsible for ensuring those containers remain valid for the
         * lifetime of the Mesh instance.
         *
         * @param vertices Reference to a vector of Vertex data used by the mesh.
         * @param indices Reference to a vector of element indices used by the mesh.
         */
        Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
        : vertices(vertices), indices(indices) {}
};

struct COMPMesh
{
    std::shared_ptr<Mesh> mesh;
};