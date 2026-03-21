#include <gtest/gtest.h>
#include <Components/Material.hpp>

// Test Material struct default construction
TEST(MaterialTest, DefaultConstruction) {
    Material material;

    // Check default texture IDs (should be -1 cast to size_t)
    EXPECT_EQ(material.diffuse, static_cast<TextureID>(-1));
    EXPECT_EQ(material.specular, static_cast<TextureID>(-1));

    // Check default ambient
    EXPECT_FLOAT_EQ(material.ambient.x, 0.1f);
    EXPECT_FLOAT_EQ(material.ambient.y, 0.1f);
    EXPECT_FLOAT_EQ(material.ambient.z, 0.1f);

    // Check default shininess
    EXPECT_FLOAT_EQ(material.shininess, 32.0f);

    // Check default transparency (fully opaque)
    EXPECT_FLOAT_EQ(material.transparency, 1.0f);
}

// Test Material struct with custom values
TEST(MaterialTest, CustomConstruction) {
    Material material;
    material.shader = 5;
    material.diffuse = 100;
    material.specular = 200;
    material.ambient = glm::vec3(0.2f, 0.3f, 0.4f);
    material.shininess = 64.0f;

    // Verify all fields
    EXPECT_EQ(material.shader, 5u);
    EXPECT_EQ(material.diffuse, 100u);
    EXPECT_EQ(material.specular, 200u);
    EXPECT_FLOAT_EQ(material.ambient.x, 0.2f);
    EXPECT_FLOAT_EQ(material.ambient.y, 0.3f);
    EXPECT_FLOAT_EQ(material.ambient.z, 0.4f);
    EXPECT_FLOAT_EQ(material.shininess, 64.0f);
}

// Test Material copy semantics
TEST(MaterialTest, CopySemantics) {
    Material mat1;
    mat1.shader = 10;
    mat1.diffuse = 50;
    mat1.specular = 75;
    mat1.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
    mat1.shininess = 128.0f;

    // Copy construct
    Material mat2 = mat1;

    // Verify copy has same values
    EXPECT_EQ(mat2.shader, 10u);
    EXPECT_EQ(mat2.diffuse, 50u);
    EXPECT_EQ(mat2.specular, 75u);
    EXPECT_FLOAT_EQ(mat2.ambient.x, 0.15f);
    EXPECT_FLOAT_EQ(mat2.ambient.y, 0.15f);
    EXPECT_FLOAT_EQ(mat2.ambient.z, 0.15f);
    EXPECT_FLOAT_EQ(mat2.shininess, 128.0f);

    // Modify copy and verify original is unchanged
    mat2.shininess = 256.0f;
    EXPECT_FLOAT_EQ(mat1.shininess, 128.0f);
    EXPECT_FLOAT_EQ(mat2.shininess, 256.0f);
}

// Test Material without textures (diffuse/specular = -1)
TEST(MaterialTest, NoTextures) {
    Material material;
    material.shader = 1;
    material.diffuse = static_cast<TextureID>(-1);
    material.specular = static_cast<TextureID>(-1);
    material.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    material.shininess = 32.0f;

    // Verify texture IDs are still -1
    EXPECT_EQ(material.diffuse, static_cast<TextureID>(-1));
    EXPECT_EQ(material.specular, static_cast<TextureID>(-1));
}

// Test Material with only diffuse texture
TEST(MaterialTest, DiffuseTextureOnly) {
    Material material;
    material.shader = 1;
    material.diffuse = 42;
    material.specular = static_cast<TextureID>(-1);

    EXPECT_EQ(material.diffuse, 42u);
    EXPECT_EQ(material.specular, static_cast<TextureID>(-1));
}

// Test Material with only specular texture
TEST(MaterialTest, SpecularTextureOnly) {
    Material material;
    material.shader = 1;
    material.diffuse = static_cast<TextureID>(-1);
    material.specular = 99;

    EXPECT_EQ(material.diffuse, static_cast<TextureID>(-1));
    EXPECT_EQ(material.specular, 99u);
}

// Test Material with both textures
TEST(MaterialTest, BothTextures) {
    Material material;
    material.shader = 1;
    material.diffuse = 123;
    material.specular = 456;

    EXPECT_EQ(material.diffuse, 123u);
    EXPECT_EQ(material.specular, 456u);
}

// Test COMPMaterial struct
TEST(COMPMaterialTest, Construction) {
    COMPMaterial comp;
    comp.material = 42;

    EXPECT_EQ(comp.material, 42u);
}

// Test COMPMaterial assignment
TEST(COMPMaterialTest, Assignment) {
    COMPMaterial comp1;
    comp1.material = 100;

    COMPMaterial comp2;
    comp2.material = 200;

    EXPECT_EQ(comp1.material, 100u);
    EXPECT_EQ(comp2.material, 200u);

    // Assignment
    comp2 = comp1;
    EXPECT_EQ(comp2.material, 100u);
}

// Test Material shininess edge cases
TEST(MaterialTest, ShininessEdgeCases) {
    Material material;
    material.shader = 1;

    // Very low shininess (dull surface)
    material.shininess = 1.0f;
    EXPECT_FLOAT_EQ(material.shininess, 1.0f);

    // Very high shininess (very shiny surface)
    material.shininess = 256.0f;
    EXPECT_FLOAT_EQ(material.shininess, 256.0f);

    // Zero shininess (edge case)
    material.shininess = 0.0f;
    EXPECT_FLOAT_EQ(material.shininess, 0.0f);
}

// Test Material ambient variations
TEST(MaterialTest, AmbientVariations) {
    Material material;
    material.shader = 1;

    // No ambient light
    material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(material.ambient.x, 0.0f);
    EXPECT_FLOAT_EQ(material.ambient.y, 0.0f);
    EXPECT_FLOAT_EQ(material.ambient.z, 0.0f);

    // Full ambient light
    material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(material.ambient.x, 1.0f);
    EXPECT_FLOAT_EQ(material.ambient.y, 1.0f);
    EXPECT_FLOAT_EQ(material.ambient.z, 1.0f);

    // Colored ambient (warm)
    material.ambient = glm::vec3(0.3f, 0.2f, 0.1f);
    EXPECT_FLOAT_EQ(material.ambient.x, 0.3f);
    EXPECT_FLOAT_EQ(material.ambient.y, 0.2f);
    EXPECT_FLOAT_EQ(material.ambient.z, 0.1f);
}

// Test realistic material properties
TEST(MaterialTest, RealisticMaterials) {
    // Plastic material
    Material plastic;
    plastic.shader = 1;
    plastic.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    plastic.shininess = 32.0f;
    EXPECT_LT(plastic.shininess, 64.0f);

    // Metal material
    Material metal;
    metal.shader = 1;
    metal.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    metal.shininess = 128.0f;
    EXPECT_GT(metal.shininess, 64.0f);

    // Rubber material (very dull)
    Material rubber;
    rubber.shader = 1;
    rubber.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    rubber.shininess = 8.0f;
    EXPECT_LT(rubber.shininess, 16.0f);
}

// ─── Transparency field tests ──────────────────────────────────────────────

// Test Material default transparency
TEST(MaterialTest, TransparencyDefaultIsOpaque) {
    Material material;
    EXPECT_FLOAT_EQ(material.transparency, 1.0f);
}

// Test setting transparency to fully transparent
TEST(MaterialTest, TransparencyFullyTransparent) {
    Material material;
    material.transparency = 0.0f;
    EXPECT_FLOAT_EQ(material.transparency, 0.0f);
}

// Test setting transparency to semi-transparent
TEST(MaterialTest, TransparencySemiTransparent) {
    Material material;
    material.transparency = 0.5f;
    EXPECT_FLOAT_EQ(material.transparency, 0.5f);
}

// Test that transparency is independent of other fields
TEST(MaterialTest, TransparencyIndependentOfOtherFields) {
    Material m1;
    m1.transparency = 0.3f;
    m1.shininess = 64.f;

    Material m2;
    m2.transparency = 0.8f;
    m2.shininess = 64.f;

    EXPECT_FLOAT_EQ(m1.shininess, m2.shininess);
    EXPECT_NE(m1.transparency, m2.transparency);
}

// Test transparency copy
TEST(MaterialTest, TransparencyCopiedCorrectly) {
    Material src;
    src.transparency = 0.25f;
    Material copy = src;
    EXPECT_FLOAT_EQ(copy.transparency, 0.25f);
    copy.transparency = 0.9f;
    EXPECT_FLOAT_EQ(src.transparency, 0.25f);
}

// Regression: transparency field exists alongside shader ID in Material
TEST(MaterialTest, TransparencyAndShaderCoexist) {
    Material material;
    material.shader = 7;
    material.transparency = 0.6f;
    EXPECT_EQ(material.shader, 7u);
    EXPECT_FLOAT_EQ(material.transparency, 0.6f);
}