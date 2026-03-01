#pragma once
#include <vector>

#include <Engine/LowLevel/Vertex.hpp>
#include <Engine/LowLevel/Shader.hpp>

struct COMPGeometry
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};