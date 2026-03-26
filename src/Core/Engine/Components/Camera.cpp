#include "Camera.hpp"
#include <iostream>

/**
 * @brief Constructs a COMPCamera with the specified projection parameters and initializes its projection matrix.
 *
 * Initializes the camera's field of view, aspect ratio, near and far clipping planes, and computes the initial
 * projection matrix from these values.
 *
 * @param fov Vertical field of view in degrees.
 * @param ratio Aspect ratio (width / height).
 * @param near Distance to the near clipping plane (must be > 0).
 * @param far Distance to the far clipping plane (must be > near).
 */
COMPCamera::COMPCamera(float fov, float ratio, float near, float far)
    : fov(fov), ratio(ratio), near(near), far(far), yaw(0.f), pitch(0.f)
{

    projection = glm::perspective(
        glm::radians(fov), 
        ratio, 
        near, 
        far
    );
}

void COMPCamera::SetFar(float n)
{
    far = n;
    dirty_camera = true;
}

void COMPCamera::SetFOV(float n)
{
    fov = n;
    dirty_camera = true;
}

void COMPCamera::SetRatio(float n)
{
    ratio = n;
    dirty_camera = true;
}

/**
 * @brief Set the camera's near clipping plane distance.
 *
 * Updates the distance from the camera to the near clipping plane and flags the camera as needing its projection/view data refreshed.
 *
 * @param n Distance to the near clipping plane (world units).
 */
void COMPCamera::SetNear(float n)
{
    near = n;
    dirty_camera = true;
}

/**
 * @brief Adjusts the camera's Euler orientation by adding yaw and pitch deltas.
 *
 * Adds the provided yaw and pitch values (in degrees) to the camera's stored yaw and pitch.
 * The resulting pitch is clamped to the range [-89.0, 89.0] degrees to avoid gimbal lock.
 *
 * @param yaw Yaw delta in degrees (rotation around the world up axis).
 * @param pitch Pitch delta in degrees (rotation around the camera's right axis); resulting pitch is clamped.
 */
void COMPCamera::RotateEuler(float yaw, float pitch)
{
    this->yaw += yaw;
    this->pitch += pitch;
    if (this->pitch > 89.0f) this->pitch = 89.0f;
	if (this->pitch < -89.0f) this->pitch = -89.0f;
}

/**
 * @brief Computes the camera's forward (look) direction from its yaw and pitch angles.
 *
 * Angles are interpreted in degrees and the returned vector is normalized.
 *
 * @return glm::vec3 Normalized forward direction vector corresponding to the current `yaw` and `pitch`.
 */
glm::vec3 COMPCamera::GetForward()
{
    glm::vec3 forward;
    forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward.y = sin(glm::radians(pitch));
    forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(forward);
}

/**
 * @brief Computes the camera's right (strafe) direction vector.
 *
 * The returned vector is a unit vector pointing to the camera's local right,
 * perpendicular to the camera forward direction and the world up axis (0, 1, 0).
 *
 * @return glm::vec3 Right unit vector pointing to the camera's local right.
 */
glm::vec3 COMPCamera::GetRight()
{
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    return glm::normalize(glm::cross(GetForward(), worldUp));
}

/**
 * @brief Computes the camera's up direction vector based on its current orientation.
 *
 * The returned vector is orthogonal to the camera's forward and right directions and represents
 * the camera's upward direction in world space.
 *
 * @return glm::vec3 Normalized up direction vector in world coordinates.
 */
glm::vec3 COMPCamera::GetUp()
{
    return glm::normalize(glm::cross(GetRight(), GetForward()));
}

/**
 * @brief Produce a CameraContext populated with this camera's projection and orientation parameters.
 *
 * @return CameraContext populated with this camera's `fov`, `ratio`, `near`, `far`, `pitch`, and `yaw`; any other fields are zero-initialized.
 */
CameraContext COMPCamera::GetCameraContext() const
{
    CameraContext context{};
    context.fov = fov;
    context.ratio = ratio;
    context.near = near;
    context.far = far;

    context.pitch = pitch;
    context.yaw = yaw;

    return context;
}
