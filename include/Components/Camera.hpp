#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <Contexts/CameraContext.hpp>

// Class, because when changing the floats inside the struct, the dirty_camera needs to be turned on
class COMPCamera
{
    float fov;
    float ratio;
    float near;
    float far;
public:
    COMPCamera(float fov, float ratio, float near, float far);
    glm::mat4 view;
    glm::mat4 projection;
    bool dirty_camera = false;

    float pitch;
    float yaw;

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