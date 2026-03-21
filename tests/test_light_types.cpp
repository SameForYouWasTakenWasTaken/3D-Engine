#include <gtest/gtest.h>
#include <Components/Light.hpp>

// ─── LightType enum ────────────────────────────────────────────────────────

TEST(LightTypeTest, EnumValuesAreDistinct) {
    EXPECT_NE(static_cast<int>(LightType::NONE), static_cast<int>(LightType::DIRECTIONAL));
    EXPECT_NE(static_cast<int>(LightType::NONE), static_cast<int>(LightType::POINT));
    EXPECT_NE(static_cast<int>(LightType::NONE), static_cast<int>(LightType::SPOT));
    EXPECT_NE(static_cast<int>(LightType::DIRECTIONAL), static_cast<int>(LightType::POINT));
    EXPECT_NE(static_cast<int>(LightType::DIRECTIONAL), static_cast<int>(LightType::SPOT));
    EXPECT_NE(static_cast<int>(LightType::POINT), static_cast<int>(LightType::SPOT));
}

// ─── COMPLight ─────────────────────────────────────────────────────────────

TEST(COMPLightTest, DefaultIdIsMaxUint32) {
    COMPLight comp;
    EXPECT_EQ(comp.id, static_cast<LightID>(-1));
}

TEST(COMPLightTest, DefaultTypeIsNone) {
    COMPLight comp;
    EXPECT_EQ(comp.type, LightType::NONE);
}

TEST(COMPLightTest, TypeCanBeSetToDirectional) {
    COMPLight comp;
    comp.type = LightType::DIRECTIONAL;
    EXPECT_EQ(comp.type, LightType::DIRECTIONAL);
}

TEST(COMPLightTest, TypeCanBeSetToPoint) {
    COMPLight comp;
    comp.type = LightType::POINT;
    EXPECT_EQ(comp.type, LightType::POINT);
}

TEST(COMPLightTest, TypeCanBeSetToSpot) {
    COMPLight comp;
    comp.type = LightType::SPOT;
    EXPECT_EQ(comp.type, LightType::SPOT);
}

TEST(COMPLightTest, CopyPreservesIdAndType) {
    COMPLight src;
    src.id = 42;
    src.type = LightType::POINT;

    COMPLight copy = src;
    EXPECT_EQ(copy.id, 42u);
    EXPECT_EQ(copy.type, LightType::POINT);
}

// ─── DirectionalLight ──────────────────────────────────────────────────────

TEST(DirectionalLightTest, DefaultDirectionIsZero) {
    DirectionalLight light;
    EXPECT_FLOAT_EQ(light.direction.x, 0.f);
    EXPECT_FLOAT_EQ(light.direction.y, 0.f);
    EXPECT_FLOAT_EQ(light.direction.z, 0.f);
}

TEST(DirectionalLightTest, DefaultColorIsWhite) {
    DirectionalLight light;
    EXPECT_FLOAT_EQ(light.color.r, 1.f);
    EXPECT_FLOAT_EQ(light.color.g, 1.f);
    EXPECT_FLOAT_EQ(light.color.b, 1.f);
}

TEST(DirectionalLightTest, DefaultIntensity) {
    DirectionalLight light;
    EXPECT_FLOAT_EQ(light.intensity, 0.5f);
}

TEST(DirectionalLightTest, DefaultDiffuse) {
    DirectionalLight light;
    EXPECT_FLOAT_EQ(light.diffuse.r, 1.f);
    EXPECT_FLOAT_EQ(light.diffuse.g, 1.f);
    EXPECT_FLOAT_EQ(light.diffuse.b, 1.f);
}

TEST(DirectionalLightTest, DefaultSpecular) {
    DirectionalLight light;
    EXPECT_FLOAT_EQ(light.specular.r, 0.5f);
    EXPECT_FLOAT_EQ(light.specular.g, 0.5f);
    EXPECT_FLOAT_EQ(light.specular.b, 0.5f);
}

TEST(DirectionalLightTest, DefaultAmbient) {
    DirectionalLight light;
    EXPECT_FLOAT_EQ(light.ambient.r, 0.1f);
    EXPECT_FLOAT_EQ(light.ambient.g, 0.1f);
    EXPECT_FLOAT_EQ(light.ambient.b, 0.1f);
}

TEST(DirectionalLightTest, GetTypeReturnsDirectional) {
    DirectionalLight light;
    EXPECT_EQ(light.GetType(), LightType::DIRECTIONAL);
}

TEST(DirectionalLightTest, DirectionCanBeSet) {
    DirectionalLight light;
    light.direction = glm::vec3(0.f, -1.f, 0.f);
    EXPECT_FLOAT_EQ(light.direction.x, 0.f);
    EXPECT_FLOAT_EQ(light.direction.y, -1.f);
    EXPECT_FLOAT_EQ(light.direction.z, 0.f);
}

// ─── PointLight ────────────────────────────────────────────────────────────

TEST(PointLightTest, DefaultPositionIsZero) {
    PointLight light;
    EXPECT_FLOAT_EQ(light.position.x, 0.f);
    EXPECT_FLOAT_EQ(light.position.y, 0.f);
    EXPECT_FLOAT_EQ(light.position.z, 0.f);
}

TEST(PointLightTest, DefaultAttenuationCoefficients) {
    PointLight light;
    EXPECT_FLOAT_EQ(light.constant, 1.0f);
    EXPECT_FLOAT_EQ(light.linear, 0.09f);
    EXPECT_FLOAT_EQ(light.quadratic, 0.032f);
}

TEST(PointLightTest, DefaultColorIsWhite) {
    PointLight light;
    EXPECT_FLOAT_EQ(light.color.r, 1.f);
    EXPECT_FLOAT_EQ(light.color.g, 1.f);
    EXPECT_FLOAT_EQ(light.color.b, 1.f);
}

TEST(PointLightTest, DefaultDiffuse) {
    PointLight light;
    EXPECT_FLOAT_EQ(light.diffuse.r, 1.f);
    EXPECT_FLOAT_EQ(light.diffuse.g, 1.f);
    EXPECT_FLOAT_EQ(light.diffuse.b, 1.f);
}

TEST(PointLightTest, DefaultAmbient) {
    PointLight light;
    EXPECT_FLOAT_EQ(light.ambient.r, 0.05f);
    EXPECT_FLOAT_EQ(light.ambient.g, 0.05f);
    EXPECT_FLOAT_EQ(light.ambient.b, 0.05f);
}

TEST(PointLightTest, GetTypeReturnsPoint) {
    PointLight light;
    EXPECT_EQ(light.GetType(), LightType::POINT);
}

TEST(PointLightTest, PositionCanBeSet) {
    PointLight light;
    light.position = glm::vec3(10.f, 5.f, -3.f);
    EXPECT_FLOAT_EQ(light.position.x, 10.f);
    EXPECT_FLOAT_EQ(light.position.y, 5.f);
    EXPECT_FLOAT_EQ(light.position.z, -3.f);
}

// ─── SpotLight ─────────────────────────────────────────────────────────────

TEST(SpotLightTest, DefaultDirectionIsZero) {
    SpotLight light;
    EXPECT_FLOAT_EQ(light.direction.x, 0.f);
    EXPECT_FLOAT_EQ(light.direction.y, 0.f);
    EXPECT_FLOAT_EQ(light.direction.z, 0.f);
}

TEST(SpotLightTest, DefaultDistance) {
    SpotLight light;
    EXPECT_FLOAT_EQ(light.distance, 3.f);
}

TEST(SpotLightTest, DefaultCutOff) {
    SpotLight light;
    EXPECT_FLOAT_EQ(light.cutOff, 30.f);
}

TEST(SpotLightTest, DefaultOuterCutOff) {
    SpotLight light;
    EXPECT_FLOAT_EQ(light.outerCutOff, 40.f);
}

TEST(SpotLightTest, OuterCutOffLargerThanCutOff) {
    // By default, outer cone angle should be larger than inner cone angle
    SpotLight light;
    EXPECT_GT(light.outerCutOff, light.cutOff);
}

TEST(SpotLightTest, GetTypeReturnsSpot) {
    SpotLight light;
    EXPECT_EQ(light.GetType(), LightType::SPOT);
}

TEST(SpotLightTest, InheritsPointLightAttenuation) {
    SpotLight light;
    EXPECT_FLOAT_EQ(light.constant, 1.0f);
    EXPECT_FLOAT_EQ(light.linear, 0.09f);
    EXPECT_FLOAT_EQ(light.quadratic, 0.032f);
}

TEST(SpotLightTest, InheritsPointLightPosition) {
    SpotLight light;
    light.position = glm::vec3(1.f, 2.f, 3.f);
    EXPECT_FLOAT_EQ(light.position.x, 1.f);
}

// ─── LightBase polymorphism ────────────────────────────────────────────────

TEST(LightBaseTest, DirectionalLightGetTypeViaPointer) {
    LightBase* light = new DirectionalLight();
    EXPECT_EQ(light->GetType(), LightType::DIRECTIONAL);
    delete light;
}

TEST(LightBaseTest, PointLightGetTypeViaPointer) {
    LightBase* light = new PointLight();
    EXPECT_EQ(light->GetType(), LightType::POINT);
    delete light;
}

TEST(LightBaseTest, SpotLightGetTypeViaPointer) {
    LightBase* light = new SpotLight();
    EXPECT_EQ(light->GetType(), LightType::SPOT);
    delete light;
}

TEST(LightBaseTest, DefaultColorIsWhite) {
    DirectionalLight light;
    LightBase& base = light;
    EXPECT_FLOAT_EQ(base.color.r, 1.f);
    EXPECT_FLOAT_EQ(base.color.g, 1.f);
    EXPECT_FLOAT_EQ(base.color.b, 1.f);
}

TEST(LightBaseTest, DefaultIntensityIsHalf) {
    PointLight light;
    LightBase& base = light;
    EXPECT_FLOAT_EQ(base.intensity, 0.5f);
}