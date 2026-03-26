#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

struct COMPTransform
{
    glm::vec3 LocalPosition = glm::vec3(0.f);
    glm::vec3 LocalScale = glm::vec3(1.f);
    glm::vec3 LocalRotation = glm::vec3(0.f);
    glm::mat4 WorldMatrix{1.f};
    bool IsUniform = true;

    void Move(const glm::vec3& translation);
    void Rotate(const glm::vec3& rotation);
    void Scale(const glm::vec3& factor);

    // Setters
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);

    glm::mat4 GetModelMatrix();
    glm::mat3 GetNormalMatrix();
};