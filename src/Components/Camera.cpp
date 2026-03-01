#include <Components/Camera.hpp>

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

CameraContext COMPCamera::GetCameraContext() const
{
    CameraContext context;
    context.fov = fov;
    context.ratio = ratio;
    context.near = near;
    context.far = far;

    return context;
}
