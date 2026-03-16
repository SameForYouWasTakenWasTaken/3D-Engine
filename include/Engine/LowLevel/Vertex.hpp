#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 tex_coords = {1.f, 1.f};
	glm::vec3 normal = {0.f, 0.f, 1.f};

	/**
	 * @brief Size in bytes of the Vertex structure.
	 *
	 * @return GLsizei Size in bytes of a Vertex.
	 */
	static constexpr GLsizei stride() noexcept {
		return sizeof(Vertex);
	}
};