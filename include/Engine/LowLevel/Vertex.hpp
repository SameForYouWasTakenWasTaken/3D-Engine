#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
    glm::vec2 tex_coords = {0.f, 0.f};
	glm::vec3 normal = {0.f, 0.f, 1.f};

	static constexpr GLsizei stride() noexcept {
		return sizeof(Vertex);
	}
};