#include <Components/Transform.hpp>

/**
 * @brief Adds a translation delta to the transform's current position.
 *
 * @param translation Vector to add to the current position.
 */
void COMPTransform::Move(const glm::vec3& translation)
{
    this->LocalPosition += translation;
}

/**
 * @brief Adds an angular offset in degrees to the transform's rotation and constrains each axis to [0, 360).
 *
 * @param rot Angular offset in degrees applied to the (x, y, z) rotation components.
 */
void COMPTransform::Rotate(const glm::vec3& rot)
{
    this->LocalRotation += rot;
    this->LocalRotation = glm::mod(LocalRotation, glm::vec3(360.f));
}

/**
 * @brief Multiplies the component's current scale by per-axis scale factors.
 *
 * Each component of the stored scale is multiplied by the corresponding component of `scale`.
 *
 * @param scale Per-axis scale factors (x, y, z) to apply.
 */
void COMPTransform::Scale(const glm::vec3& scale)
{
    this->LocalScale *= scale;
}

/**
 * @brief Sets the transform's absolute position to the specified vector.
 *
 * @param position Target position (x, y, z) in the transform's coordinate space.
 */
void COMPTransform::SetPosition(const glm::vec3& position)
{
    this->LocalPosition = glm::vec3(0.f);
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
    this->LocalRotation = glm::vec3(0.f);
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
    this->LocalScale = glm::vec3(1.f);
    Scale(scale); // Scale by the difference between the new scale and the current scale
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

    model = glm::translate(model, LocalPosition);

    model = glm::rotate(model, glm::radians(LocalRotation.y), {0,1,0});
    model = glm::rotate(model, glm::radians(LocalRotation.x), {1,0,0});
    model = glm::rotate(model, glm::radians(LocalRotation.z), {0,0,1});

    model = glm::scale(model, LocalScale);

    return model;
}

/**
 * @brief Computes the 3×3 normal transformation matrix for this transform.
 *
 * The normal matrix equals the model matrix's upper-left 3×3 when the component's
 * scale is approximately uniform (each pair of scale components differs by less
 * than 0.0001); otherwise it is the inverse-transpose of that 3×3.
 *
 * @return glm::mat3 The matrix used to transform surface normals. 
 */
glm::mat3 COMPTransform::GetNormalMatrix()
{
    glm::mat4 model = GetModelMatrix();

    constexpr float uniformTolerance = 0.0001f;

    bool isUniform =
        std::abs(LocalScale.x - LocalScale.y) < uniformTolerance &&
        std::abs(LocalScale.y - LocalScale.z) < uniformTolerance;

    if (isUniform)
        return glm::mat3(model);

    return glm::transpose(glm::inverse(glm::mat3(model)));
}