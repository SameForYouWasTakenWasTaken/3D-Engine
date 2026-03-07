#include <Components/Transform.hpp>

/**
 * @brief Adds a translation delta to the transform's current position.
 *
 * @param translation Vector to add to the current position.
 */
void COMPTransform::Move(const glm::vec3& translation)
{
    this->position += translation;
}

/**
 * @brief Adds an angular offset in degrees to the transform's rotation and constrains each axis to [0, 360).
 *
 * @param rot Angular offset in degrees applied to the (x, y, z) rotation components.
 */
void COMPTransform::Rotate(const glm::vec3& rot)
{
    this->rotation += rot;
    this->rotation = glm::mod(rotation, glm::vec3(360.f));
}

void COMPTransform::Scale(const glm::vec3& scale)
{
    this->scale += scale;
}

/**
 * @brief Sets the transform's absolute position to the specified vector.
 *
 * @param position Target position (x, y, z) in the transform's coordinate space.
 */
void COMPTransform::SetPosition(const glm::vec3& position)
{
    this->position = glm::vec3(0.f);
    Move(position);
}

/**
 * @brief Sets the component's rotation to the specified Euler angles in degrees.
 *
 * Resets the stored rotation to zero and applies the provided Euler angles;
 * the resulting rotation components are normalized into the range [0, 360).
 *
 * @param rotation Euler angles in degrees (x: rotation about X / pitch, y: rotation about Y / yaw, z: rotation about Z / roll).
 */
void COMPTransform::SetRotation(const glm::vec3& rotation)
{
    this->rotation = glm::vec3(0.f);
    Rotate(rotation);
}

/**
 * @brief Sets the transform's scale to the given absolute scale.
 *
 * Resets the internal scale to (1, 1, 1) and then adjusts it so the component's
 * resulting scale equals the provided `scale` vector.
 *
 * @param scale Desired absolute scale for the transform.
 */
void COMPTransform::SetScale(const glm::vec3& scale)
{
    this->scale = glm::vec3(1.f);
    Scale(scale - this->scale); // Scale by the difference between the new scale and the current scale
}

/**
 * @brief Builds the 4x4 model transformation matrix from the component's position, rotation, and scale.
 *
 * The matrix is constructed by applying translation first, then rotations in Y, X, Z order
 * (rotation components are interpreted as degrees and converted to radians), and finally scaling.
 *
 * @return glm::mat4 4x4 transformation matrix that transforms model-space coordinates to world-space.
 */
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