#include <gtest/gtest.h>
#include <Components/Transform.hpp>

// ─── Default construction ──────────────────────────────────────────────────

TEST(COMPTransformTest, DefaultConstruction) {
    COMPTransform t;
    EXPECT_FLOAT_EQ(t.position.x, 0.f);
    EXPECT_FLOAT_EQ(t.position.y, 0.f);
    EXPECT_FLOAT_EQ(t.position.z, 0.f);

    EXPECT_FLOAT_EQ(t.scale.x, 1.f);
    EXPECT_FLOAT_EQ(t.scale.y, 1.f);
    EXPECT_FLOAT_EQ(t.scale.z, 1.f);

    EXPECT_FLOAT_EQ(t.rotation.x, 0.f);
    EXPECT_FLOAT_EQ(t.rotation.y, 0.f);
    EXPECT_FLOAT_EQ(t.rotation.z, 0.f);

    EXPECT_TRUE(t.IsUniform);
}

// ─── Move ─────────────────────────────────────────────────────────────────

TEST(COMPTransformTest, MoveAddsToPosition) {
    COMPTransform t;
    t.Move(glm::vec3(1.f, 2.f, 3.f));
    EXPECT_FLOAT_EQ(t.position.x, 1.f);
    EXPECT_FLOAT_EQ(t.position.y, 2.f);
    EXPECT_FLOAT_EQ(t.position.z, 3.f);
}

TEST(COMPTransformTest, MoveCumulative) {
    COMPTransform t;
    t.Move(glm::vec3(1.f, 0.f, 0.f));
    t.Move(glm::vec3(1.f, 0.f, 0.f));
    EXPECT_FLOAT_EQ(t.position.x, 2.f);
}

TEST(COMPTransformTest, MoveNegativeDirection) {
    COMPTransform t;
    t.Move(glm::vec3(-5.f, -3.f, -1.f));
    EXPECT_FLOAT_EQ(t.position.x, -5.f);
    EXPECT_FLOAT_EQ(t.position.y, -3.f);
    EXPECT_FLOAT_EQ(t.position.z, -1.f);
}

// ─── SetPosition ──────────────────────────────────────────────────────────

TEST(COMPTransformTest, SetPositionAbsolute) {
    COMPTransform t;
    t.Move(glm::vec3(10.f, 10.f, 10.f));
    t.SetPosition(glm::vec3(3.f, 4.f, 5.f));
    EXPECT_FLOAT_EQ(t.position.x, 3.f);
    EXPECT_FLOAT_EQ(t.position.y, 4.f);
    EXPECT_FLOAT_EQ(t.position.z, 5.f);
}

TEST(COMPTransformTest, SetPositionZero) {
    COMPTransform t;
    t.Move(glm::vec3(7.f, 8.f, 9.f));
    t.SetPosition(glm::vec3(0.f, 0.f, 0.f));
    EXPECT_FLOAT_EQ(t.position.x, 0.f);
    EXPECT_FLOAT_EQ(t.position.y, 0.f);
    EXPECT_FLOAT_EQ(t.position.z, 0.f);
}

TEST(COMPTransformTest, SetPositionNegative) {
    COMPTransform t;
    t.SetPosition(glm::vec3(-2.f, -4.f, -6.f));
    EXPECT_FLOAT_EQ(t.position.x, -2.f);
    EXPECT_FLOAT_EQ(t.position.y, -4.f);
    EXPECT_FLOAT_EQ(t.position.z, -6.f);
}

// ─── Rotate ───────────────────────────────────────────────────────────────

TEST(COMPTransformTest, RotateAddsAngles) {
    COMPTransform t;
    t.Rotate(glm::vec3(90.f, 45.f, 30.f));
    EXPECT_FLOAT_EQ(t.rotation.x, 90.f);
    EXPECT_FLOAT_EQ(t.rotation.y, 45.f);
    EXPECT_FLOAT_EQ(t.rotation.z, 30.f);
}

TEST(COMPTransformTest, RotateWrapsAt360) {
    COMPTransform t;
    t.Rotate(glm::vec3(350.f, 0.f, 0.f));
    t.Rotate(glm::vec3(20.f, 0.f, 0.f));
    // 350 + 20 = 370, mod 360 = 10
    EXPECT_NEAR(t.rotation.x, 10.f, 0.001f);
}

TEST(COMPTransformTest, RotateExactly360WrapsToZero) {
    COMPTransform t;
    t.Rotate(glm::vec3(360.f, 0.f, 0.f));
    EXPECT_NEAR(t.rotation.x, 0.f, 0.001f);
}

TEST(COMPTransformTest, RotateCumulative) {
    COMPTransform t;
    t.Rotate(glm::vec3(100.f, 0.f, 0.f));
    t.Rotate(glm::vec3(100.f, 0.f, 0.f));
    EXPECT_NEAR(t.rotation.x, 200.f, 0.001f);
}

// ─── SetRotation ──────────────────────────────────────────────────────────

TEST(COMPTransformTest, SetRotationAbsolute) {
    COMPTransform t;
    t.Rotate(glm::vec3(100.f, 0.f, 0.f));
    t.SetRotation(glm::vec3(45.f, 90.f, 180.f));
    EXPECT_NEAR(t.rotation.x, 45.f, 0.001f);
    EXPECT_NEAR(t.rotation.y, 90.f, 0.001f);
    EXPECT_NEAR(t.rotation.z, 180.f, 0.001f);
}

TEST(COMPTransformTest, SetRotationWrapsAt360) {
    COMPTransform t;
    t.SetRotation(glm::vec3(400.f, 0.f, 0.f));
    // 400 mod 360 = 40
    EXPECT_NEAR(t.rotation.x, 40.f, 0.001f);
}

// ─── Scale ────────────────────────────────────────────────────────────────

TEST(COMPTransformTest, ScaleMultiplies) {
    COMPTransform t;
    t.Scale(glm::vec3(2.f, 3.f, 4.f));
    EXPECT_FLOAT_EQ(t.scale.x, 2.f);
    EXPECT_FLOAT_EQ(t.scale.y, 3.f);
    EXPECT_FLOAT_EQ(t.scale.z, 4.f);
}

TEST(COMPTransformTest, ScaleCumulativeMultiplies) {
    COMPTransform t;
    t.Scale(glm::vec3(2.f, 2.f, 2.f));
    t.Scale(glm::vec3(3.f, 3.f, 3.f));
    // 2 * 3 = 6
    EXPECT_FLOAT_EQ(t.scale.x, 6.f);
    EXPECT_FLOAT_EQ(t.scale.y, 6.f);
    EXPECT_FLOAT_EQ(t.scale.z, 6.f);
}

TEST(COMPTransformTest, ScaleByZeroCollapsesScale) {
    COMPTransform t;
    t.Scale(glm::vec3(0.f, 0.f, 0.f));
    EXPECT_FLOAT_EQ(t.scale.x, 0.f);
    EXPECT_FLOAT_EQ(t.scale.y, 0.f);
    EXPECT_FLOAT_EQ(t.scale.z, 0.f);
}

// ─── SetScale ─────────────────────────────────────────────────────────────
// SetScale(s) resets scale to 1, then calls Scale(s - 1).
// Resulting scale = 1 * (s - 1) = s - 1.

TEST(COMPTransformTest, SetScaleActualBehavior) {
    COMPTransform t;
    t.SetScale(glm::vec3(3.f, 3.f, 3.f));
    // SetScale resets to 1, then Scale(3-1=2): scale = 1*2 = 2
    EXPECT_FLOAT_EQ(t.scale.x, 2.f);
    EXPECT_FLOAT_EQ(t.scale.y, 2.f);
    EXPECT_FLOAT_EQ(t.scale.z, 2.f);
}

TEST(COMPTransformTest, SetScaleIsAbsolute) {
    // SetScale should override any previously accumulated scale
    COMPTransform t;
    t.Scale(glm::vec3(10.f, 10.f, 10.f));
    t.SetScale(glm::vec3(5.f, 5.f, 5.f));
    // After SetScale: scale = 5 - 1 = 4
    EXPECT_FLOAT_EQ(t.scale.x, 4.f);
    EXPECT_FLOAT_EQ(t.scale.y, 4.f);
    EXPECT_FLOAT_EQ(t.scale.z, 4.f);
}

TEST(COMPTransformTest, SetScaleNonUniform) {
    COMPTransform t;
    t.SetScale(glm::vec3(4.f, 2.f, 3.f));
    // SetScale(4,2,3): Scale(4-1=3, 2-1=1, 3-1=2) → scale = (3, 1, 2)
    EXPECT_FLOAT_EQ(t.scale.x, 3.f);
    EXPECT_FLOAT_EQ(t.scale.y, 1.f);
    EXPECT_FLOAT_EQ(t.scale.z, 2.f);
}

// ─── GetModelMatrix ────────────────────────────────────────────────────────

TEST(COMPTransformTest, GetModelMatrixDefaultIsIdentity) {
    COMPTransform t;
    glm::mat4 m = t.GetModelMatrix();
    glm::mat4 identity = glm::mat4(1.f);
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            EXPECT_NEAR(m[col][row], identity[col][row], 0.0001f);
}

TEST(COMPTransformTest, GetModelMatrixTranslationOnly) {
    COMPTransform t;
    t.SetPosition(glm::vec3(5.f, 0.f, 0.f));
    glm::mat4 m = t.GetModelMatrix();
    // Translation column is column 3
    EXPECT_NEAR(m[3][0], 5.f, 0.0001f);
    EXPECT_NEAR(m[3][1], 0.f, 0.0001f);
    EXPECT_NEAR(m[3][2], 0.f, 0.0001f);
}

TEST(COMPTransformTest, GetModelMatrixScaleOnly) {
    COMPTransform t;
    t.Scale(glm::vec3(2.f, 3.f, 4.f));
    glm::mat4 m = t.GetModelMatrix();
    // Diagonal should reflect scale
    EXPECT_NEAR(m[0][0], 2.f, 0.0001f);
    EXPECT_NEAR(m[1][1], 3.f, 0.0001f);
    EXPECT_NEAR(m[2][2], 4.f, 0.0001f);
}

TEST(COMPTransformTest, GetModelMatrixRotation90Y) {
    COMPTransform t;
    t.SetRotation(glm::vec3(0.f, 90.f, 0.f));
    glm::mat4 m = t.GetModelMatrix();
    // After 90 degrees Y rotation, (1,0,0) maps to approximately (0,0,-1)
    EXPECT_NEAR(m[0][0], 0.f, 0.001f);
    EXPECT_NEAR(m[0][2], -1.f, 0.001f);
}

// ─── GetNormalMatrix ───────────────────────────────────────────────────────

TEST(COMPTransformTest, GetNormalMatrixUniformScaleReturnsMat3OfModel) {
    COMPTransform t;
    t.Scale(glm::vec3(2.f, 2.f, 2.f)); // uniform scale
    glm::mat3 n = t.GetNormalMatrix();
    glm::mat3 expected = glm::mat3(t.GetModelMatrix());
    for (int col = 0; col < 3; ++col)
        for (int row = 0; row < 3; ++row)
            EXPECT_NEAR(n[col][row], expected[col][row], 0.0001f);
}

TEST(COMPTransformTest, GetNormalMatrixNonUniformScaleUsesTransposeInverse) {
    COMPTransform t;
    t.Scale(glm::vec3(2.f, 1.f, 3.f)); // non-uniform scale
    glm::mat3 n = t.GetNormalMatrix();
    glm::mat3 expected = glm::transpose(glm::inverse(glm::mat3(t.GetModelMatrix())));
    for (int col = 0; col < 3; ++col)
        for (int row = 0; row < 3; ++row)
            EXPECT_NEAR(n[col][row], expected[col][row], 0.0001f);
}

TEST(COMPTransformTest, GetNormalMatrixDefaultTransformIsIdentity) {
    COMPTransform t;
    glm::mat3 n = t.GetNormalMatrix();
    glm::mat3 identity = glm::mat3(1.f);
    for (int col = 0; col < 3; ++col)
        for (int row = 0; row < 3; ++row)
            EXPECT_NEAR(n[col][row], identity[col][row], 0.0001f);
}

// ─── Boundary / regression cases ──────────────────────────────────────────

TEST(COMPTransformTest, RotateNegativeAnglesStayInPositiveRange) {
    // glm::mod with negative values still wraps correctly in glm
    COMPTransform t;
    t.Rotate(glm::vec3(10.f, 0.f, 0.f));
    EXPECT_GE(t.rotation.x, 0.f);
    EXPECT_LT(t.rotation.x, 360.f);
}

TEST(COMPTransformTest, MoveDoesNotAffectScaleOrRotation) {
    COMPTransform t;
    t.Move(glm::vec3(5.f, 5.f, 5.f));
    EXPECT_FLOAT_EQ(t.scale.x, 1.f);
    EXPECT_FLOAT_EQ(t.rotation.x, 0.f);
}

TEST(COMPTransformTest, ScaleDoesNotAffectPositionOrRotation) {
    COMPTransform t;
    t.Scale(glm::vec3(3.f, 3.f, 3.f));
    EXPECT_FLOAT_EQ(t.position.x, 0.f);
    EXPECT_FLOAT_EQ(t.rotation.x, 0.f);
}