#pragma once

#include <vendor/glad.h>
#include "../Vertex.hpp"

struct AttributePointer
{
    GLuint index; // attribute layout location in the shader. Is it the position, color, texture coords, etc.
    GLint size = 3; // how many numbers make up the attribute? (e.g., 2 for vec2, 3 for vec3, etc.)
    GLenum type = GL_FLOAT; // data type of each component, default GL_FLOAT
    GLboolean normalized = GL_FALSE;
    GLsizei stride = Vertex::stride(); // Suppose each vertex has 6 floats: 3 for position, 3 for color, then stride is 6 * sizeof(float)
    const void* offset; // position(x,y,z) | color(r,g,b), the color starts after the 3rd float (after z), so the offset for the RGB is 3
};

class VAO final
{
    GLsizei amnt; // VAO size
public:
    GLuint id;

    VAO(GLsizei n = 1);
    ~VAO();
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    VAO(VAO&& other) noexcept : id(other.id) {
        other.id = 0;
    }
    void LinkAttrib(AttributePointer attrib);
    void Bind();
    static void Unbind();
};