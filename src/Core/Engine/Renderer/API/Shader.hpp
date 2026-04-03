#pragma once
#include <vendor/glad.h>
#include <string>

#include <glm/glm.hpp>
#include <optional>
#include <utility>

#include "vendor/Logger.hpp"

using ShaderID = uint64_t;

std::optional<std::pair<std::string, std::string>>
ParseShaderFiles(const std::string&  FragmentSourceFilePath, const std::string&  VertexSourceFilePath);

class Shader final
{
    GLuint m_Program, m_VertexShader, m_FragmentShader;
    std::string VertexShaderSource, FragmentShaderSource; // Normal files
    std::string GLSL_FragmentShaderSource, GLSL_VertexShaderSource; // GLSL files
public:
    Shader(const std::string& VertexShaderSource, const std::string& FragmentShaderSource);
    ~Shader();

    void ResetShaders();
    void ResetShaders(const std::string& VertexShaderFilepath, const std::string& FragmentShaderFilepath);
    void SetShaderSources(const std::string& VertexShaderFilepath, const std::string& FragmentShaderSource); // This does NOT take in file paths!
    void UseProgram();
    void UnuseProgram();

    // Uniform functions
    void SetInt(const std::string& name, int n);
    void SetFloat(const std::string& name, float n);
    void SetVec3(const std::string& name, const glm::vec3& vec);
    void SetVec2(const std::string& name, const glm::vec2& vec);
    
    void SetMatrix4(const std::string& name, int amount, const GLfloat* value);
    void SetMatrix3(const std::string& name, int amount, const GLfloat* value);
    void SetMatrix4(int loc, int amount, const GLfloat* value);

    int GetUniformLocation(const std::string& name);
    std::pair<std::string, std::string> GetFilepaths() const;

    Logger logger = Logger("Shader");
};