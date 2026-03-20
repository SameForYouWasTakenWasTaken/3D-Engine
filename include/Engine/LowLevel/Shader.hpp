#pragma once
#include <glad/glad.h>
#include <string>

#include <glm/glm.hpp>
#include <Logger.hpp>

std::optional<std::pair<std::string, std::string>>
ParseShaderFiles(const std::string&  FragmentSourceFilePath, const std::string&  VertexSourceFilePath);

class Shader final
{
    GLuint m_Program, m_VertexShader, m_FragmentShader;
    std::string GLSL_FragmentShaderSource, GLSL_VertexShaderSource;
public:
    Shader(const std::string& VertexShaderSource, const std::string& FragmentShaderSource);
    ~Shader();

    void ResetShaders();
    void ResetShaders(const std::string& VertexShaderFilepath, const std::string& FragmentShaderFilepath);
    void SetShaderSources(const std::string& VertexShaderFilepath, const std::string& FragmentShaderSource); // This does NOT take in file paths!
    void UseProgram();
    void UnuseProgram();
    // ? Uniform functions ?
    void SetInt(const std::string& name, int n);
    void SetFloat(const std::string& name, float n);
    void SetVec3(const std::string& name, const glm::vec3& vec);
    
    void SetMatrix4(const std::string& name, int amount, const GLfloat* value);
    void SetMatrix3(const std::string& name, int amount, const GLfloat* value);
    void SetMatrix4(int loc, int amount, const GLfloat* value);

    int GetUniformLocation(const std::string& name);

    Logger logger = Logger("Shader");
};