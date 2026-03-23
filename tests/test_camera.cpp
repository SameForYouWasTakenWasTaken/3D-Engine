#include <gtest/gtest.h>
#include <Components/Camera.hpp>
#include <cmath>

// ─── Construction ─────────────────────────────────────────────────────────

TEST(COMPCameraTest, ConstructionStoresParameters) {
    COMPCamera cam(90.f, 16.f / 9.f, 0.1f, 100.f);

    CameraContext ctx = cam.GetCameraContext();
    EXPECT_FLOAT_EQ(ctx.fov, 90.f);
    EXPECT_FLOAT_EQ(ctx.ratio, 16.f / 9.f);
    EXPECT_FLOAT_EQ(ctx.near, 0.1f);
    EXPECT_FLOAT_EQ(ctx.far, 100.f);
}

TEST(COMPCameraTest, ConstructionInitializesDirtyFalse) {
    COMPCamera cam(60.f, 1.f, 0.1f, 1000.f);
    EXPECT_FALSE(cam.dirty_camera);
}

TEST(COMPCameraTest, ConstructionInitializesPitchYawToZero) {
    COMPCamera cam(45.f, 1.33f, 0.01f, 500.f);
    EXPECT_FLOAT_EQ(cam.pitch, 0.f);
    EXPECT_FLOAT_EQ(cam.yaw, 0.f);
}

TEST(COMPCameraTest, ConstructionBuildsProjectionMatrix) {
    COMPCamera cam(90.f, 16.f / 9.f, 0.1f, 100.f);
    // Projection matrix should not be the zero matrix
    bool allZero = true;
    for (int c = 0; c < 4 && allZero; ++c)
        for (int r = 0; r < 4 && allZero; ++r)
            if (cam.projection[c][r] != 0.f) allZero = false;
    EXPECT_FALSE(allZero);
}

// ─── SetFOV ───────────────────────────────────────────────────────────────

TEST(COMPCameraTest, SetFOVMarksDirty) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    EXPECT_FALSE(cam.dirty_camera);
    cam.SetFOV(60.f);
    EXPECT_TRUE(cam.dirty_camera);
}

TEST(COMPCameraTest, SetFOVUpdatesContext) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetFOV(45.f);
    EXPECT_FLOAT_EQ(cam.GetCameraContext().fov, 45.f);
}

// ─── SetRatio ─────────────────────────────────────────────────────────────

TEST(COMPCameraTest, SetRatioMarksDirty) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetRatio(16.f / 9.f);
    EXPECT_TRUE(cam.dirty_camera);
}

TEST(COMPCameraTest, SetRatioUpdatesContext) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetRatio(2.f);
    EXPECT_FLOAT_EQ(cam.GetCameraContext().ratio, 2.f);
}

// ─── SetNear ──────────────────────────────────────────────────────────────

TEST(COMPCameraTest, SetNearMarksDirty) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetNear(0.5f);
    EXPECT_TRUE(cam.dirty_camera);
}

TEST(COMPCameraTest, SetNearUpdatesContext) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetNear(1.0f);
    EXPECT_FLOAT_EQ(cam.GetCameraContext().near, 1.0f);
}

// ─── SetFar ───────────────────────────────────────────────────────────────

TEST(COMPCameraTest, SetFarMarksDirty) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetFar(500.f);
    EXPECT_TRUE(cam.dirty_camera);
}

TEST(COMPCameraTest, SetFarUpdatesContext) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetFar(200.f);
    EXPECT_FLOAT_EQ(cam.GetCameraContext().far, 200.f);
}

// ─── RotateEuler ──────────────────────────────────────────────────────────

TEST(COMPCameraTest, RotateEulerAccumulatesYaw) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(45.f, 0.f);
    cam.RotateEuler(30.f, 0.f);
    EXPECT_FLOAT_EQ(cam.yaw, 75.f);
}

TEST(COMPCameraTest, RotateEulerAccumulatesPitch) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(0.f, 20.f);
    cam.RotateEuler(0.f, 10.f);
    EXPECT_FLOAT_EQ(cam.pitch, 30.f);
}

TEST(COMPCameraTest, RotateEulerClampsPitchAtPositive89) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(0.f, 100.f);
    EXPECT_FLOAT_EQ(cam.pitch, 89.f);
}

TEST(COMPCameraTest, RotateEulerClampsPitchAtNegative89) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(0.f, -100.f);
    EXPECT_FLOAT_EQ(cam.pitch, -89.f);
}

TEST(COMPCameraTest, RotateEulerPitchExactly89NotClamped) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(0.f, 89.f);
    EXPECT_FLOAT_EQ(cam.pitch, 89.f);
}

TEST(COMPCameraTest, RotateEulerPitchExactlyMinus89NotClamped) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(0.f, -89.f);
    EXPECT_FLOAT_EQ(cam.pitch, -89.f);
}

// ─── GetForward ───────────────────────────────────────────────────────────

TEST(COMPCameraTest, GetForwardDefaultYawPitchPointsAlongPosX) {
    // yaw=0, pitch=0 → forward = (cos(0)*cos(0), sin(0), sin(0)*cos(0)) = (1,0,0)
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    glm::vec3 fwd = cam.GetForward();
    EXPECT_NEAR(fwd.x, 1.f, 0.001f);
    EXPECT_NEAR(fwd.y, 0.f, 0.001f);
    EXPECT_NEAR(fwd.z, 0.f, 0.001f);
}

TEST(COMPCameraTest, GetForwardIsNormalized) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(45.f, 30.f);
    glm::vec3 fwd = cam.GetForward();
    float len = glm::length(fwd);
    EXPECT_NEAR(len, 1.f, 0.001f);
}

TEST(COMPCameraTest, GetForwardYaw90PointsAlongPosZ) {
    // yaw=90, pitch=0: x=cos(90)*cos(0)=0, y=sin(0)=0, z=sin(90)*cos(0)=1
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(90.f, 0.f);
    glm::vec3 fwd = cam.GetForward();
    EXPECT_NEAR(fwd.x, 0.f, 0.001f);
    EXPECT_NEAR(fwd.y, 0.f, 0.001f);
    EXPECT_NEAR(fwd.z, 1.f, 0.001f);
}

// ─── GetRight ─────────────────────────────────────────────────────────────

TEST(COMPCameraTest, GetRightIsNormalized) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(30.f, 15.f);
    glm::vec3 right = cam.GetRight();
    EXPECT_NEAR(glm::length(right), 1.f, 0.001f);
}

TEST(COMPCameraTest, GetRightIsPerpendicularToForward) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(45.f, 20.f);
    glm::vec3 fwd = cam.GetForward();
    glm::vec3 right = cam.GetRight();
    float dot = glm::dot(fwd, right);
    EXPECT_NEAR(dot, 0.f, 0.001f);
}

TEST(COMPCameraTest, GetRightDefaultYawPointsAlongPosZ) {
    // forward=(1,0,0), worldUp=(0,1,0) → right=cross(fwd,up)=(0*0-0*1, 0*1-1*0, 1*1-0*0)=(0,0,1)
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    glm::vec3 right = cam.GetRight();
    EXPECT_NEAR(right.x, 0.f, 0.001f);
    EXPECT_NEAR(right.y, 0.f, 0.001f);
    EXPECT_NEAR(right.z, 1.f, 0.001f);
}

// ─── GetUp ────────────────────────────────────────────────────────────────

TEST(COMPCameraTest, GetUpIsNormalized) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(30.f, 15.f);
    glm::vec3 up = cam.GetUp();
    EXPECT_NEAR(glm::length(up), 1.f, 0.001f);
}

TEST(COMPCameraTest, GetUpIsPerpendicularToForwardAndRight) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.RotateEuler(45.f, 20.f);
    glm::vec3 fwd = cam.GetForward();
    glm::vec3 right = cam.GetRight();
    glm::vec3 up = cam.GetUp();
    EXPECT_NEAR(glm::dot(up, fwd), 0.f, 0.001f);
    EXPECT_NEAR(glm::dot(up, right), 0.f, 0.001f);
}

TEST(COMPCameraTest, GetUpDefaultOrientationIsWorldUp) {
    // At yaw=0, pitch=0: forward=(1,0,0), right=(0,0,1)
    // up = cross(right, fwd) = cross((0,0,1),(1,0,0)) = (0*0-1*0, 1*1-0*0, 0*0-0*1) = (0,1,0)
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    glm::vec3 up = cam.GetUp();
    EXPECT_NEAR(up.x, 0.f, 0.001f);
    EXPECT_NEAR(up.y, 1.f, 0.001f);
    EXPECT_NEAR(up.z, 0.f, 0.001f);
}

// ─── GetCameraContext ─────────────────────────────────────────────────────

TEST(COMPCameraTest, GetCameraContextReflectsCurrentState) {
    COMPCamera cam(60.f, 1.5f, 0.05f, 500.f);
    cam.RotateEuler(30.f, 15.f);

    CameraContext ctx = cam.GetCameraContext();
    EXPECT_FLOAT_EQ(ctx.fov, 60.f);
    EXPECT_FLOAT_EQ(ctx.ratio, 1.5f);
    EXPECT_FLOAT_EQ(ctx.near, 0.05f);
    EXPECT_FLOAT_EQ(ctx.far, 500.f);
    EXPECT_FLOAT_EQ(ctx.yaw, 30.f);
    EXPECT_FLOAT_EQ(ctx.pitch, 15.f);
}

TEST(COMPCameraTest, GetCameraContextAfterSetFOV) {
    COMPCamera cam(90.f, 1.f, 0.1f, 100.f);
    cam.SetFOV(30.f);
    EXPECT_FLOAT_EQ(cam.GetCameraContext().fov, 30.f);
}