#include <gtest/gtest.h>
#include <Engine/LowLevel/Vertex.hpp>

// Test Vertex struct default construction
TEST(VertexTest, DefaultConstruction) {
    Vertex vertex{};
    vertex.position = glm::vec3(0.0f);
    vertex.color = glm::vec4(0.0f);

    // Check default tex_coords
    EXPECT_FLOAT_EQ(vertex.tex_coords.x, 1.0f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.y, 1.0f);

    // Check default normal (pointing in +Z direction)
    EXPECT_FLOAT_EQ(vertex.normal.x, 0.0f);
    EXPECT_FLOAT_EQ(vertex.normal.y, 0.0f);
    EXPECT_FLOAT_EQ(vertex.normal.z, 1.0f);
}

// Test Vertex struct custom construction
TEST(VertexTest, CustomConstruction) {
    Vertex vertex;
    vertex.position = glm::vec3(1.0f, 2.0f, 3.0f);
    vertex.color = glm::vec4(0.5f, 0.6f, 0.7f, 0.8f);
    vertex.tex_coords = glm::vec2(0.25f, 0.75f);
    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);

    // Verify all fields
    EXPECT_FLOAT_EQ(vertex.position.x, 1.0f);
    EXPECT_FLOAT_EQ(vertex.position.y, 2.0f);
    EXPECT_FLOAT_EQ(vertex.position.z, 3.0f);

    EXPECT_FLOAT_EQ(vertex.color.r, 0.5f);
    EXPECT_FLOAT_EQ(vertex.color.g, 0.6f);
    EXPECT_FLOAT_EQ(vertex.color.b, 0.7f);
    EXPECT_FLOAT_EQ(vertex.color.a, 0.8f);

    EXPECT_FLOAT_EQ(vertex.tex_coords.x, 0.25f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.y, 0.75f);

    EXPECT_FLOAT_EQ(vertex.normal.x, 0.0f);
    EXPECT_FLOAT_EQ(vertex.normal.y, 1.0f);
    EXPECT_FLOAT_EQ(vertex.normal.z, 0.0f);
}

// Test Vertex stride() method
TEST(VertexTest, StrideMethod) {
    GLsizei stride = Vertex::stride();

    // Stride should equal sizeof(Vertex)
    EXPECT_EQ(stride, sizeof(Vertex));

    // Stride should be positive
    EXPECT_GT(stride, 0);

    // Verify stride is what we expect for the structure
    // vec3 (12 bytes) + vec4 (16 bytes) + vec2 (8 bytes) + vec3 (12 bytes) = 48 bytes
    EXPECT_EQ(stride, 48);
}

// Test Vertex stride() is constexpr and noexcept
TEST(VertexTest, StrideConstexprNoexcept) {
    // This should compile as constexpr
    constexpr GLsizei stride = Vertex::stride();
    EXPECT_EQ(stride, sizeof(Vertex));

    // Verify noexcept (compile-time check)
    static_assert(noexcept(Vertex::stride()), "stride() should be noexcept");
}

// Test Vertex copy semantics
TEST(VertexTest, CopySemantics) {
    Vertex v1;
    v1.position = glm::vec3(5.0f, 6.0f, 7.0f);
    v1.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    v1.tex_coords = glm::vec2(0.5f, 0.5f);
    v1.normal = glm::vec3(-1.0f, 0.0f, 0.0f);

    // Copy construct
    Vertex v2 = v1;

    // Verify copy has same values
    EXPECT_FLOAT_EQ(v2.position.x, 5.0f);
    EXPECT_FLOAT_EQ(v2.position.y, 6.0f);
    EXPECT_FLOAT_EQ(v2.position.z, 7.0f);

    EXPECT_FLOAT_EQ(v2.color.r, 1.0f);
    EXPECT_FLOAT_EQ(v2.color.g, 0.0f);
    EXPECT_FLOAT_EQ(v2.color.b, 0.0f);
    EXPECT_FLOAT_EQ(v2.color.a, 1.0f);

    EXPECT_FLOAT_EQ(v2.tex_coords.x, 0.5f);
    EXPECT_FLOAT_EQ(v2.tex_coords.y, 0.5f);

    EXPECT_FLOAT_EQ(v2.normal.x, -1.0f);
    EXPECT_FLOAT_EQ(v2.normal.y, 0.0f);
    EXPECT_FLOAT_EQ(v2.normal.z, 0.0f);

    // Modify copy and verify original is unchanged
    v2.position.x = 100.0f;
    EXPECT_FLOAT_EQ(v1.position.x, 5.0f);
    EXPECT_FLOAT_EQ(v2.position.x, 100.0f);
}

// Test Vertex texture coordinate edge cases
TEST(VertexTest, TextureCoordinateEdgeCases) {
    Vertex vertex;
    vertex.position = glm::vec3(0.0f);
    vertex.color = glm::vec4(1.0f);
    vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);

    // Bottom-left corner (0, 0)
    vertex.tex_coords = glm::vec2(0.0f, 0.0f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.x, 0.0f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.y, 0.0f);

    // Top-right corner (1, 1)
    vertex.tex_coords = glm::vec2(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.x, 1.0f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.y, 1.0f);

    // Values outside [0,1] range (for wrapping/clamping)
    vertex.tex_coords = glm::vec2(2.0f, -1.0f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.x, 2.0f);
    EXPECT_FLOAT_EQ(vertex.tex_coords.y, -1.0f);
}

// Test Vertex normal vectors
TEST(VertexTest, NormalVectors) {
    Vertex vertex;
    vertex.position = glm::vec3(0.0f);
    vertex.color = glm::vec4(1.0f);
    vertex.tex_coords = glm::vec2(0.0f);

    // Test cardinal direction normals
    // +X
    vertex.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(glm::length(vertex.normal), 1.0f);

    // -X
    vertex.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(glm::length(vertex.normal), 1.0f);

    // +Y
    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    EXPECT_FLOAT_EQ(glm::length(vertex.normal), 1.0f);

    // -Y
    vertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    EXPECT_FLOAT_EQ(glm::length(vertex.normal), 1.0f);

    // +Z
    vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    EXPECT_FLOAT_EQ(glm::length(vertex.normal), 1.0f);

    // -Z
    vertex.normal = glm::vec3(0.0f, 0.0f, -1.0f);
    EXPECT_FLOAT_EQ(glm::length(vertex.normal), 1.0f);
}

// Test Vertex with normalized normal
TEST(VertexTest, NormalizedNormal) {
    Vertex vertex;
    vertex.position = glm::vec3(0.0f);
    vertex.color = glm::vec4(1.0f);
    vertex.tex_coords = glm::vec2(0.0f);

    // Set a non-normalized normal
    glm::vec3 unnormalized(1.0f, 2.0f, 3.0f);
    vertex.normal = glm::normalize(unnormalized);

    // Check that it's normalized (length = 1)
    float length = glm::length(vertex.normal);
    EXPECT_NEAR(length, 1.0f, 0.0001f);
}

// Test realistic vertex data for a triangle
TEST(VertexTest, TriangleVertices) {
    // Create three vertices for a triangle
    Vertex v1, v2, v3;

    // Bottom-left
    v1.position = glm::vec3(-1.0f, -1.0f, 0.0f);
    v1.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    v1.tex_coords = glm::vec2(0.0f, 0.0f);
    v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);

    // Bottom-right
    v2.position = glm::vec3(1.0f, -1.0f, 0.0f);
    v2.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    v2.tex_coords = glm::vec2(1.0f, 0.0f);
    v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);

    // Top-center
    v3.position = glm::vec3(0.0f, 1.0f, 0.0f);
    v3.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    v3.tex_coords = glm::vec2(0.5f, 1.0f);
    v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);

    // All vertices should have the same normal (coplanar)
    EXPECT_EQ(v1.normal, v2.normal);
    EXPECT_EQ(v2.normal, v3.normal);

    // Different colors
    EXPECT_NE(v1.color, v2.color);
    EXPECT_NE(v2.color, v3.color);

    // Different positions
    EXPECT_NE(v1.position, v2.position);
    EXPECT_NE(v2.position, v3.position);
}

// Test Vertex array layout for OpenGL
TEST(VertexTest, MemoryLayout) {
    Vertex vertex;

    // Check that fields are laid out sequentially
    // This is important for OpenGL vertex attribute pointers
    const void* vertex_addr = &vertex;
    const void* position_addr = &vertex.position;
    const void* color_addr = &vertex.color;
    const void* tex_coords_addr = &vertex.tex_coords;
    const void* normal_addr = &vertex.normal;

    // Position should be at offset 0
    EXPECT_EQ(position_addr, vertex_addr);

    // Each field should follow the previous
    ptrdiff_t color_offset = static_cast<const char*>(color_addr) - static_cast<const char*>(vertex_addr);
    ptrdiff_t tex_offset = static_cast<const char*>(tex_coords_addr) - static_cast<const char*>(vertex_addr);
    ptrdiff_t normal_offset = static_cast<const char*>(normal_addr) - static_cast<const char*>(vertex_addr);

    // Position (vec3) = 12 bytes, so color should start at offset 12
    EXPECT_EQ(color_offset, sizeof(glm::vec3));

    // Color (vec4) = 16 bytes, so tex_coords should start at offset 12 + 16 = 28
    EXPECT_EQ(tex_offset, sizeof(glm::vec3) + sizeof(glm::vec4));

    // tex_coords (vec2) = 8 bytes, so normal should start at offset 12 + 16 + 8 = 36
    EXPECT_EQ(normal_offset, sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec2));
}