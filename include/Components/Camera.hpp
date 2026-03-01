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

    void SetFOV(float n);
    void SetRatio(float n);
    void SetNear(float n);
    void SetFar(float n);

    CameraContext GetCameraContext() const;
};