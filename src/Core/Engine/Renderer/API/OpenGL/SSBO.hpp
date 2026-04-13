#pragma once
#include <vector>

#include "API_Util/GPUData.hpp"
#include "vendor/glad.h"

class SSBO final
{
public:
    GLuint id;

    SSBO();
    ~SSBO();
    SSBO(const SSBO&) = delete;
    SSBO& operator=(const SSBO&) = delete;

    SSBO(SSBO&& other) noexcept : id(other.id) {
        other.id = 0;
    }

    void SetBufferData(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW);
    void Bind();
    void SetBinding(int n);
    static void Unbind();

    template <typename GPUData> requires(std::is_class_v<GPUData>)
    void UploadGPUData(std::vector<GPUData>& data, GLenum usage = GL_DYNAMIC_DRAW);
};

template <typename GPUData> requires (std::is_class_v<GPUData>)
void SSBO::UploadGPUData(std::vector<GPUData>& data, GLenum usage)
{
    Bind();
    SetBufferData(
    static_cast<GLsizeiptr>(data.size() * sizeof(GPUData)),
    data.data(),
    usage
    );
    Unbind();
}
