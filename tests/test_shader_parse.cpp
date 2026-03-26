#include <gtest/gtest.h>
#include <../src/Core/Engine/Renderer/API/Shader.hpp>
#include <fstream>
#include <filesystem>

// Temp directory to store temporary files
const std::string temp_dir = std::filesystem::temp_directory_path().string();

// Helper function to create a test shader file
void CreateTestShaderFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    file.close();
}

// Helper function to delete a test shader file
void DeleteTestShaderFile(const std::string& path) {
    std::filesystem::remove(path);
}

// Test ParseShaderFiles with valid files
TEST(ParseShaderFilesTest, ValidFiles) {
    // Create test shader files
    const std::string vert_path = temp_dir + "/test_shader.vert";
    const std::string frag_path = temp_dir + "/test_shader.frag";

    const std::string vert_content = "#version 330 core\nlayout(location = 0) in vec3 aPos;\nvoid main() { gl_Position = vec4(aPos, 1.0); }";
    const std::string frag_content = "#version 330 core\nout vec4 FragColor;\nvoid main() { FragColor = vec4(1.0, 0.5, 0.2, 1.0); }";

    CreateTestShaderFile(vert_path, vert_content);
    CreateTestShaderFile(frag_path, frag_content);

    // Parse the shader files
    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    // Should succeed
    ASSERT_TRUE(result.has_value());

    // Verify contents
    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, vert_content);
    EXPECT_EQ(fragment_source, frag_content);

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles with missing vertex shader
TEST(ParseShaderFilesTest, MissingVertexShader) {
    const std::string vert_path = temp_dir + "/nonexistent_vertex.vert";
    const std::string frag_path = temp_dir + "/test_shader.frag";

    CreateTestShaderFile(frag_path, "fragment shader content");

    // Parse with missing vertex shader
    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    // Should fail
    EXPECT_FALSE(result.has_value());

    // Clean up
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles with missing fragment shader
TEST(ParseShaderFilesTest, MissingFragmentShader) {
    const std::string vert_path = temp_dir + "/test_shader.vert";
    const std::string frag_path = temp_dir + "/nonexistent_fragment.frag";

    CreateTestShaderFile(vert_path, "vertex shader content");

    // Parse with missing fragment shader
    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    // Should fail
    EXPECT_FALSE(result.has_value());

    // Clean up
    DeleteTestShaderFile(vert_path);
}

// Test ParseShaderFiles with both files missing
TEST(ParseShaderFilesTest, BothFilesMissing) {
    const std::string vert_path = temp_dir + "/nonexistent_vertex.vert";
    const std::string frag_path = temp_dir + "/nonexistent_fragment.frag";

    // Parse with both files missing
    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    // Should fail
    EXPECT_FALSE(result.has_value());
}

// Test ParseShaderFiles with empty files
TEST(ParseShaderFilesTest, EmptyFiles) {
    const std::string vert_path = temp_dir + "/empty_vertex.vert";
    const std::string frag_path = temp_dir + "/empty_fragment.frag";

    CreateTestShaderFile(vert_path, "");
    CreateTestShaderFile(frag_path, "");

    // Parse empty shader files
    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    // Should succeed (empty files are valid)
    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, "");
    EXPECT_EQ(fragment_source, "");

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles with multiline shaders
TEST(ParseShaderFilesTest, MultilineShaders) {
    const std::string vert_path = temp_dir + "/multiline_vertex.vert";
    const std::string frag_path = temp_dir + "/multiline_fragment.frag";

    const std::string vert_content =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}";

    const std::string frag_content =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 objectColor;\n"
        "uniform vec3 lightColor;\n"
        "void main() {\n"
        "    FragColor = vec4(lightColor * objectColor, 1.0);\n"
        "}";

    CreateTestShaderFile(vert_path, vert_content);
    CreateTestShaderFile(frag_path, frag_content);

    // Parse the shader files
    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, vert_content);
    EXPECT_EQ(fragment_source, frag_content);

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles with special characters
TEST(ParseShaderFilesTest, SpecialCharacters) {
    const std::string vert_path = temp_dir + "/special_vertex.vert";
    const std::string frag_path = temp_dir + "/special_fragment.frag";

    const std::string vert_content = "// Comment with special chars: @#$%^&*()\n#version 330 core\nvoid main() {}";
    const std::string frag_content = "/* Multi-line comment\n   with special chars: <>?[]{}|\\~`*/\nvoid main() {}";

    CreateTestShaderFile(vert_path, vert_content);
    CreateTestShaderFile(frag_path, frag_content);

    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, vert_content);
    EXPECT_EQ(fragment_source, frag_content);

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles with large files
TEST(ParseShaderFilesTest, LargeFiles) {
    const std::string vert_path = temp_dir + "/large_vertex.vert";
    const std::string frag_path = temp_dir + "/large_fragment.frag";

    // Create large shader content (repetitive but valid)
    std::string vert_content = "#version 330 core\n";
    std::string frag_content = "#version 330 core\n";

    for (int i = 0; i < 100; ++i) {
        vert_content += "// Comment line " + std::to_string(i) + "\n";
        frag_content += "// Comment line " + std::to_string(i) + "\n";
    }

    vert_content += "void main() { gl_Position = vec4(0.0); }";
    frag_content += "void main() { FragColor = vec4(1.0); }";

    CreateTestShaderFile(vert_path, vert_content);
    CreateTestShaderFile(frag_path, frag_content);

    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, vert_content);
    EXPECT_EQ(fragment_source, frag_content);

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles with same file for both shaders
TEST(ParseShaderFilesTest, SameFileForBoth) {
    const std::string shader_path = temp_dir + "/same_shader.glsl";
    const std::string content = "#version 330 core\nvoid main() {}";

    CreateTestShaderFile(shader_path, content);

    // Parse with same file for both vertex and fragment
    auto result = ParseShaderFiles(shader_path.c_str(), shader_path.c_str());

    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, content);
    EXPECT_EQ(fragment_source, content);
    EXPECT_EQ(vertex_source, fragment_source);

    // Clean up
    DeleteTestShaderFile(shader_path);
}

// Test ParseShaderFiles with whitespace-only files
TEST(ParseShaderFilesTest, WhitespaceOnlyFiles) {
    const std::string vert_path = temp_dir + "/whitespace_vertex.vert";
    const std::string frag_path = temp_dir + "/whitespace_fragment.frag";

    const std::string vert_content = "   \n\t\n   ";
    const std::string frag_content = "\n\n\n\t\t\t";

    CreateTestShaderFile(vert_path, vert_content);
    CreateTestShaderFile(frag_path, frag_content);

    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, vert_content);
    EXPECT_EQ(fragment_source, frag_content);

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles with newline variations
TEST(ParseShaderFilesTest, NewlineVariations) {
    const std::string vert_path = temp_dir + "/newline_vertex.vert";
    const std::string frag_path = temp_dir + "/newline_fragment.frag";

    const std::string vert_content = "line1\nline2\nline3";
    const std::string frag_content = "line1\nline2\nline3\n";

    CreateTestShaderFile(vert_path, vert_content);
    CreateTestShaderFile(frag_path, frag_content);

    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();
    EXPECT_EQ(vertex_source, vert_content);
    EXPECT_EQ(fragment_source, frag_content);

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}

// Test ParseShaderFiles preserves exact content
TEST(ParseShaderFilesTest, PreservesExactContent) {
    const std::string vert_path = temp_dir + "/exact_vertex.vert";
    const std::string frag_path = temp_dir + "/exact_fragment.frag";

    // Content with various edge cases
    const std::string vert_content = "  leading spaces\ntrailing spaces  \n\ttabs\t\n";
    const std::string frag_content = "MixedCase123\n!@#$%^&*()\n\"quotes\"\n'apostrophes'";

    CreateTestShaderFile(vert_path, vert_content);
    CreateTestShaderFile(frag_path, frag_content);

    auto result = ParseShaderFiles(vert_path.c_str(), frag_path.c_str());

    ASSERT_TRUE(result.has_value());

    auto [vertex_source, fragment_source] = result.value();

    // Content should be exactly preserved
    EXPECT_EQ(vertex_source, vert_content);
    EXPECT_EQ(fragment_source, frag_content);

    // Clean up
    DeleteTestShaderFile(vert_path);
    DeleteTestShaderFile(frag_path);
}