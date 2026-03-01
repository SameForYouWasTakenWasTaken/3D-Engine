#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <glad/glad.h>

struct Vertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 tex_coords = {};

	static constexpr GLsizei stride() noexcept {
		return static_cast<GLsizei>(
			sizeof(Vertex)
		);
	}
};