#include <Components/Transform.hpp>

void COMPTransform::Move(const glm::vec3& translation)
{
    this->position += translation;
}

void COMPTransform::Rotate(const glm::vec3& rot)
{
    this->rotation += rot;
    this->rotation = glm::mod(rotation, glm::vec3(360.f));
}

void COMPTransform::Scale(const glm::vec3& scale)
{
    this->scale += scale;
}

void COMPTransform::SetPosition(const glm::vec3& position)
{
    this->position = glm::vec3(0.f);
    Move(position);
}

void COMPTransform::SetRotation(const glm::vec3& rotation)
{
    this->rotation = glm::vec3(0.f);
    Rotate(rotation);
}

void COMPTransform::SetScale(const glm::vec3& scale)
{
    this->scale = glm::vec3(1.f);
    Scale(scale - this->scale); // Scale by the difference between the new scale and the current scale
}

glm::mat4 COMPTransform::GetModelMatrix()
{
    glm::mat4 model = glm::mat4(1.f);

    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotation.y), {0,1,0});
    model = glm::rotate(model, glm::radians(rotation.x), {1,0,0});
    model = glm::rotate(model, glm::radians(rotation.z), {0,0,1});

    model = glm::scale(model, scale);

    return model;
}