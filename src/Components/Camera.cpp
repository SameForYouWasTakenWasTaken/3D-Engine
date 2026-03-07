#include <Components/Camera.hpp>
#include <iostream>

COMPCamera::COMPCamera(float fov, float ratio, float near, float far)
    : fov(fov), ratio(ratio), near(near), far(far) 
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

void COMPCamera::SetNear(float n)
{
    near = n;
    dirty_camera = true;
}

void COMPCamera::RotateEuler(float yaw, float pitch)
{
    this->yaw += yaw;
    this->pitch += pitch;
    if (this->pitch > 89.0f) this->pitch = 89.0f;
	if (this->pitch < -89.0f) this->pitch = -89.0f;
}

glm::vec3 COMPCamera::GetForward()
{
    glm::vec3 forward;
    forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward.y = sin(glm::radians(pitch));
    forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(forward);
}

glm::vec3 COMPCamera::GetRight()
{
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    return glm::normalize(glm::cross(GetForward(), worldUp));
}

glm::vec3 COMPCamera::GetUp()
{
    return glm::normalize(glm::cross(GetRight(), GetForward()));
}

CameraContext COMPCamera::GetCameraContext() const
{
    CameraContext context;
    context.fov = fov;
    context.ratio = ratio;
    context.near = near;
    context.far = far;

    context.pitch = pitch;
    context.yaw = yaw;

    return context;
}
