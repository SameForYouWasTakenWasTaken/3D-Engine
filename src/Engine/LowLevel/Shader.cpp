#include <Engine/LowLevel/Shader.hpp>

std::expected<std::pair<std::string, std::string>, bool>
ParseShaderFiles(const std::string& VertexSourceFilePath, const std::string& FragmentSourceFilePath)
{
    std::ifstream vertex(VertexSourceFilePath); // vertex contents
    std::ifstream fragment(FragmentSourceFilePath); // fragment contents

    if (!fragment.is_open() || !vertex.is_open()) {
        return std::unexpected(false);
    }

    std::stringstream frag_buffer;
    std::stringstream vert_buffer;


    frag_buffer << fragment.rdbuf();
    vert_buffer << vertex.rdbuf();

    return std::make_pair(vert_buffer.str(), frag_buffer.str());
}

Shader::Shader(const std::string& VertexSourceFilePath, const std::string& FragmentSourceFilePath)
{
    auto result = ParseShaderFiles(VertexSourceFilePath, FragmentSourceFilePath);
    if(!result)
    {
        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("Couldn't retrieve fragment/vertex source files!");
        return;
    }

    auto ShaderSources = result.value();

    SetShaderSources(ShaderSources.first.c_str(), ShaderSources.second.c_str());
    ResetShaders(); // Forward to resetshaders, since it already handles all of the shader creation
}

Shader::~Shader()
{
    if (glIsProgram(m_Program) == GL_TRUE)
        glDeleteProgram(m_Program);

    // Usually not the case, since they *should* get deleted anyway, but honestly it doesnt hurt to put this here
    if (glIsShader(m_VertexShader) == GL_TRUE)
        glDeleteShader(m_VertexShader);
    
    if (glIsShader(m_FragmentShader) == GL_TRUE)
        glDeleteShader(m_FragmentShader);
}

void Shader::ResetShaders(const std::string& VertexSourceFilepath, const std::string& FragmentSourceFilepath)
{
    auto result = ParseShaderFiles(VertexSourceFilepath, FragmentSourceFilepath);
    if(!result)
    {
        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("Couldn't retrieve fragment/vertex source files!");
        return;
    }

    auto ShaderSources = result.value();
    SetShaderSources(ShaderSources.first.c_str(), ShaderSources.second.c_str());
    ResetShaders(); // Forward to resetshaders, since it already handles all of the shader creation
}

void Shader::ResetShaders()
{
    if (GLSL_FragmentShaderSource.empty() || GLSL_VertexShaderSource.empty())
    {
        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("Fragment or vertex shader source is empty!");
        return;
    }

    if (glIsProgram(m_Program) == GL_TRUE)
        glDeleteProgram(m_Program);

    // Convert to const chars because OpenGL
    const char* GLSL_VertSource = GLSL_VertexShaderSource.c_str();
    const char* GLSL_FragSource = GLSL_FragmentShaderSource.c_str();
        
    // Vertex
    m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_VertexShader, 1, &GLSL_VertSource, NULL);
    glCompileShader(m_VertexShader);

    // Fragment
    m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_FragmentShader, 1, &GLSL_FragSource, NULL);
    glCompileShader(m_FragmentShader);


    // Program
    m_Program = glCreateProgram();
    glAttachShader(m_Program, m_VertexShader);
    glAttachShader(m_Program, m_FragmentShader);

    glLinkProgram(m_Program);

    // Debug    
    int success;
    char infoLog[512];
    std::stringstream ss;
    glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(m_FragmentShader, GL_COMPILE_STATUS, &success);
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    
    if(!success)
    {
        glGetShaderInfoLog(m_VertexShader, 512, NULL, infoLog);
        glGetShaderInfoLog(m_FragmentShader, 512, NULL, infoLog);
        glGetProgramInfoLog(m_Program, 512, NULL, infoLog);
        
        ss << infoLog;
        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + ss.str());
    }

    glDeleteShader(m_VertexShader);
    glDeleteShader(m_FragmentShader);
}

void Shader::UseProgram()
{
    logger.DumpLogs(); // Dump all logs before using the shader
    if (glIsProgram(m_Program) == GL_FALSE)
    {
        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("Shader program not available?");
        return;
    }
    glUseProgram(m_Program);
}

void Shader::UnuseProgram()
{
    if (glIsProgram(m_Program) == GL_FALSE)
    {
        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("Shader program not available?");
        return;
    }

    glUseProgram(0); // ! If you put this in the if statement above regardless, it might unuse a different shader program which might be currently in use that isnt this one !
}

void Shader::SetShaderSources(const std::string& m_VertexShaderSource, const std::string& m_FragmentShaderSource)
{
    GLSL_VertexShaderSource = m_VertexShaderSource;
    GLSL_FragmentShaderSource = m_FragmentShaderSource;
}

void Shader::SetInt(const std::string& name, int n)
{
    int loc = GetUniformLocation(name);
    if (loc != -1)
        glUniform1i(loc, n);
}

void Shader::SetFloat(const std::string& name, float n)
{
    int loc = GetUniformLocation(name);
    if (loc != -1)
        glUniform1f(loc, n);
}    
    
// Typically reserved for one time uses
void Shader::SetMatrix4(const std::string& name, int amount, const GLfloat* value)
{
    int loc = GetUniformLocation(name);
    if (loc != -1)
        glUniformMatrix4fv(loc, amount, GL_FALSE, value);
}

void Shader::SetMatrix3(const std::string& name, int amount, const GLfloat* value)
{
    int loc = GetUniformLocation(name);
    if (loc != -1)
        glUniformMatrix3fv(loc, amount, GL_FALSE, value);
}
// Typically reserved if you already have a location and simply don't want to rewrite the whole function.
// Either way, looks prettier. Totally optional, of course.
void Shader::SetMatrix4(int loc, int amount, const GLfloat* value)
{
    glUniformMatrix4fv(loc, amount, GL_FALSE, value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& vec)
{
    int loc = GetUniformLocation(name);
    if (loc != -1)
        glUniform3f(loc, vec.x, vec.y, vec.z);
}


int Shader::GetUniformLocation(const std::string& name)
{
    if (glIsProgram(m_Program))
        return glGetUniformLocation(m_Program, name.c_str());

    return -1;
}