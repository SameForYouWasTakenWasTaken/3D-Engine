#pragma once
#include <vector>

#include <vendor/glad.h>

class VBO final {
public:
    GLuint id;

    VBO();
    ~VBO();

    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    VBO(VBO&& other) noexcept : id(other.id) {
        other.id = 0;
    }

    template<typename T>
    void SetData(std::vector<T>& vertices, GLenum draw_type = GL_STATIC_DRAW);

    void Bind();
    static void Unbind();

};

template <typename T>
/**
 * @brief Uploads a contiguous array of vertex data into the currently bound GL_ARRAY_BUFFER.
 *
 * The buffer must be bound (e.g. with Bind()) before calling this method. The contents of
 * `vertices` are copied into the buffer and the usage hint `draw_type` is set for the data store.
 *
 * @tparam T Element type stored in `vertices` (e.g. a vertex struct or scalar type).
 * @param vertices Vector containing the data to upload; its raw bytes are copied into the buffer.
 * @param draw_type OpenGL usage hint (e.g. GL_STATIC_DRAW, GL_DYNAMIC_DRAW).
 */
void VBO::SetData(std::vector<T>& vertices, GLenum draw_type)
{
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), draw_type);
}
