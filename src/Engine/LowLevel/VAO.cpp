#include <Engine/LowLevel/VAO.hpp>

VAO::VAO(GLsizei n)
: amnt(n)
{
    glGenVertexArrays(amnt, &id);
}

VAO::~VAO()
{
    glDeleteVertexArrays(amnt, &id);
}

void VAO::Bind()
{
    glBindVertexArray(id);
}

void VAO::Unbind()
{
    glBindVertexArray(0);
}

void VAO::LinkAttrib(AttributePointer attrib)
{
    glVertexAttribPointer(
        attrib.index, 
        attrib.size, 
        attrib.type, 
        attrib.normalized, 
        attrib.stride, 
        attrib.offset);
    glEnableVertexAttribArray(attrib.index);
}