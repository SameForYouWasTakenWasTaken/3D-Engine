#pragma once
#include <memory>

#include <glm/mat4x4.hpp>
#include <Engine/LowLevel/VAO.hpp>
#include <Engine/LowLevel/VBO.hpp>
#include <Engine/LowLevel/EBO.hpp>

inline const AttributePointer GetAttribPointerPos()
{
    AttributePointer pos;
    pos.index = 0;
    pos.size = 3;
    pos.offset = (void*)offsetof(Vertex, position);
    return pos;
}

inline const AttributePointer GetAttribPointerCol()
{
    AttributePointer col;
    col.index = 1;
    col.size = 4;
    col.offset = (void*)offsetof(Vertex, color);
    return col;
}

inline const AttributePointer GetAttribPointerTex()
{
    AttributePointer tex;
    tex.index = 2;
    tex.size = 2;
    tex.offset = (void*)offsetof(Vertex, tex_coords);
    return tex;
}

struct Mesh
{
    VAO vao;
    VBO vbo;
    EBO ebo;
    
    uint32_t IndexCount;
    uint32_t VertexCount;

    VBO instanceVBO; // For instanced rendering

    bool Indexed = false;
    GLenum Primitive = GL_TRIANGLES;
    void SetData(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLenum draw_type = GL_STATIC_DRAW);
};

struct COMPMesh
{
    std::shared_ptr<Mesh> mesh;
};