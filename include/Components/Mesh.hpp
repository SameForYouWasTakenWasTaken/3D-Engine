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

inline const AttributePointer GetAttribPointerPos()
{
    AttributePointer pos;
    pos.index = 0;
    pos.size = 3;
    pos.offset = reinterpret_cast<void*>(offsetof(Vertex, position));
    return pos;
}

inline const AttributePointer GetAttribPointerCol()
{
    AttributePointer col;
    col.index = 1;
    col.size = 4;
    col.offset = reinterpret_cast<void*>(offsetof(Vertex, color));
    return col;
}

inline const AttributePointer GetAttribPointeraNormal()
{
    AttributePointer normal;
    normal.index = 3;
    normal.size = 3;
    normal.offset = reinterpret_cast<void*>(offsetof(Vertex, normal));
    return normal;
}

inline const AttributePointer GetAttribPointerTex()
{
    AttributePointer tex;
    tex.index = 2;
    tex.size = 2;
    tex.offset = reinterpret_cast<void*>(offsetof(Vertex, tex_coords));
    return tex;
}

inline const AttributePointer GetAttribPointerNormal0()
{
    AttributePointer attr{};
    attr.index = 8; // matches shader layout location
    attr.size = 4;  // vec4
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal0));
    return attr;
}

inline const AttributePointer GetAttribPointerNormal1()
{
    AttributePointer attr{};
    attr.index = 9;
    attr.size = 4;
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal1));
    return attr;
}

inline const AttributePointer GetAttribPointerNormal2()
{
    AttributePointer attr{};
    attr.index = 10;
    attr.size = 4;
    attr.offset = reinterpret_cast<void*>(offsetof(InstanceData, normal2));
    return attr;
}


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

    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices)
        :
    vertices(std::move(vertices)), indices(std::move(indices)),
    IndexCount(0), VertexCount(0)
    {
    } 
};

struct COMPMesh
{
    std::shared_ptr<Mesh> mesh;
};