#include <gtest/gtest.h>
#include <Components/DirectionalLight.hpp>

// Test Light struct default construction
TEST(LightTest, DefaultConstruction) {
    DirectionalLight light;

    // Check default position
    EXPECT_FLOAT_EQ(light.position.x, 0.0f);
    EXPECT_FLOAT_EQ(light.position.y, 0.0f);
    EXPECT_FLOAT_EQ(light.position.z, 0.0f);

    // Check default color
    EXPECT_FLOAT_EQ(light.color.x, 1.0f);
    EXPECT_FLOAT_EQ(light.color.y, 1.0f);
    EXPECT_FLOAT_EQ(light.color.z, 1.0f);

    // Check default diffuse
    EXPECT_FLOAT_EQ(light.diffuse.x, 1.0f);
    EXPECT_FLOAT_EQ(light.diffuse.y, 1.0f);
    EXPECT_FLOAT_EQ(light.diffuse.z, 1.0f);

    // Check default specular
    EXPECT_FLOAT_EQ(light.specular.x, 0.5f);
    EXPECT_FLOAT_EQ(light.specular.y, 0.5f);
    EXPECT_FLOAT_EQ(light.specular.z, 0.5f);

    // Check default ambient
    EXPECT_FLOAT_EQ(light.ambient.x, 0.1f);
    EXPECT_FLOAT_EQ(light.ambient.y, 0.1f);
    EXPECT_FLOAT_EQ(light.ambient.z, 0.1f);

    // Check default intensity
    EXPECT_FLOAT_EQ(light.intensity, 0.5f);
}

// Test Light struct custom construction
TEST(LightTest, CustomConstruction) {
    DirectionalLight light;
    light.position = glm::vec3(1.0f, 2.0f, 3.0f);
    light.color = glm::vec3(0.5f, 0.6f, 0.7f);
    light.diffuse = glm::vec3(0.8f, 0.9f, 1.0f);
    light.specular = glm::vec3(0.2f, 0.3f, 0.4f);
    light.ambient = glm::vec3(0.05f, 0.06f, 0.07f);
    light.intensity = 0.75f;

    // Verify all fields were set correctly
    EXPECT_FLOAT_EQ(light.position.x, 1.0f);
    EXPECT_FLOAT_EQ(light.position.y, 2.0f);
    EXPECT_FLOAT_EQ(light.position.z, 3.0f);

    EXPECT_FLOAT_EQ(light.color.x, 0.5f);
    EXPECT_FLOAT_EQ(light.color.y, 0.6f);
    EXPECT_FLOAT_EQ(light.color.z, 0.7f);

    EXPECT_FLOAT_EQ(light.diffuse.x, 0.8f);
    EXPECT_FLOAT_EQ(light.diffuse.y, 0.9f);
    EXPECT_FLOAT_EQ(light.diffuse.z, 1.0f);

    EXPECT_FLOAT_EQ(light.specular.x, 0.2f);
    EXPECT_FLOAT_EQ(light.specular.y, 0.3f);
    EXPECT_FLOAT_EQ(light.specular.z, 0.4f);

    EXPECT_FLOAT_EQ(light.ambient.x, 0.05f);
    EXPECT_FLOAT_EQ(light.ambient.y, 0.06f);
    EXPECT_FLOAT_EQ(light.ambient.z, 0.07f);

    EXPECT_FLOAT_EQ(light.intensity, 0.75f);
}

// Test Light copy semantics
TEST(LightTest, CopySemantics) {
    DirectionalLight light1;
    light1.position = glm::vec3(5.0f, 6.0f, 7.0f);
    light1.color = glm::vec3(0.1f, 0.2f, 0.3f);
    light1.intensity = 0.9f;

    // Copy construct
    DirectionalLight light2 = light1;

    // Verify copy has same values
    EXPECT_FLOAT_EQ(light2.position.x, 5.0f);
    EXPECT_FLOAT_EQ(light2.position.y, 6.0f);
    EXPECT_FLOAT_EQ(light2.position.z, 7.0f);
    EXPECT_FLOAT_EQ(light2.color.x, 0.1f);
    EXPECT_FLOAT_EQ(light2.color.y, 0.2f);
    EXPECT_FLOAT_EQ(light2.color.z, 0.3f);
    EXPECT_FLOAT_EQ(light2.intensity, 0.9f);

    // Modify copy and verify original is unchanged
    light2.intensity = 0.1f;
    EXPECT_FLOAT_EQ(light1.intensity, 0.9f);
    EXPECT_FLOAT_EQ(light2.intensity, 0.1f);
}

// Test COMPLight struct
TEST(COMPLightTest, DefaultConstruction) {
    COMPLight comp;
    // LightID is uint32_t, uninitialized value is implementation-defined
    // Just verify we can create and access the field
    comp.id = 42;
    EXPECT_EQ(comp.id, 42u);
}

// Test COMPLight assignment
TEST(COMPLightTest, Assignment) {
    COMPLight comp1;
    comp1.id = 100;

    COMPLight comp2;
    comp2.id = 200;

    EXPECT_EQ(comp1.id, 100u);
    EXPECT_EQ(comp2.id, 200u);

    // Assignment
    comp2 = comp1;
    EXPECT_EQ(comp2.id, 100u);
}

// Test edge cases for Light properties
TEST(LightTest, EdgeCases) {
    DirectionalLight light;

    // Test zero intensity
    light.intensity = 0.0f;
    EXPECT_FLOAT_EQ(light.intensity, 0.0f);

    // Test maximum intensity
    light.intensity = 1.0f;
    EXPECT_FLOAT_EQ(light.intensity, 1.0f);

    // Test negative position (valid for lights)
    light.position = glm::vec3(-10.0f, -20.0f, -30.0f);
    EXPECT_FLOAT_EQ(light.position.x, -10.0f);
    EXPECT_FLOAT_EQ(light.position.y, -20.0f);
    EXPECT_FLOAT_EQ(light.position.z, -30.0f);

    // Test black color (all zeros)
    light.color = glm::vec3(0.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(light.color.x, 0.0f);
    EXPECT_FLOAT_EQ(light.color.y, 0.0f);
    EXPECT_FLOAT_EQ(light.color.z, 0.0f);
}

// Test realistic lighting scenarios
TEST(LightTest, RealisticLightingScenarios) {
    // Test a warm white light
    DirectionalLight warmLight;
    warmLight.color = glm::vec3(1.0f, 0.9f, 0.7f);
    warmLight.intensity = 0.8f;
    EXPECT_GT(warmLight.color.x, warmLight.color.z);

    // Test a cool white light
    DirectionalLight coolLight;
    coolLight.color = glm::vec3(0.7f, 0.8f, 1.0f);
    coolLight.intensity = 0.6f;
    EXPECT_GT(coolLight.color.z, coolLight.color.x);

    // Test colored light (red)
    DirectionalLight redLight;
    redLight.color = glm::vec3(1.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(redLight.color.x, 1.0f);
    EXPECT_FLOAT_EQ(redLight.color.y, 0.0f);
    EXPECT_FLOAT_EQ(redLight.color.z, 0.0f);
}