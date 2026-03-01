#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct COMPTransform
{
    glm::mat4 model = glm::mat4(1.f);
    
    glm::vec3 position = glm::vec3(0.f);
    glm::vec3 scale = glm::vec3(1.f);
    glm::vec3 rotation = glm::vec3(0.f);

    void Translate(const glm::vec3& translation);
    void Rotate(const glm::vec3& rotation);
    void Scale(const glm::vec3& scale);

    // Setters
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);
};