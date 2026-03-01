#include <Components/Transform.hpp>

void COMPTransform::Translate(const glm::vec3& translation)
{
    position += translation;
    model = glm::translate(model, position);
}

void COMPTransform::Rotate(const glm::vec3& rot)
{
    this->rotation += rot;
    this->rotation = glm::mod(rotation, glm::vec3(360.f));

    model = glm::rotate(model, glm::radians(rot.x), {1,0,0});
    model = glm::rotate(model, glm::radians(rot.y), {0,1,0});
    model = glm::rotate(model, glm::radians(rot.z), {0,0,1});
}

void COMPTransform::Scale(const glm::vec3& scale)
{
    this->scale += scale;
    model = glm::scale(model, scale);
}

void COMPTransform::SetPosition(const glm::vec3& position)
{
    this->position = glm::vec3(0.f);
    Translate(position);
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