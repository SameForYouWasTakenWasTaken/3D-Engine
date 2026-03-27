#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <Contexts/CameraContext.hpp>

// Class, because when changing the floats inside the struct, the dirty_camera needs to be turned on
class COMPCamera
{
    float fov = 90.f;
    float ratio;
    float near = 0.1f;
    float far = 1000.f;
public:
    COMPCamera(float fov, float ratio, float near, float far);
    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    bool dirty_camera = false;

    float pitch = 0;
    float yaw = 0;

    void SetFOV(float n);
    void SetRatio(float n);
    void SetNear(float n);
    void SetFar(float n);
    void RotateEuler(float yaw, float pitch);

    CameraContext GetCameraContext() const;
    glm::vec3 GetForward();
    glm::vec3 GetRight();
    glm::vec3 GetUp();
};