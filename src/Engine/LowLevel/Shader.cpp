#include <Engine/LowLevel/Shader.hpp>

/**
 * @brief Loads vertex and fragment shader source text from two files.
 *
 * Reads the entire contents of the file at VertexSourceFilePath as the vertex
 * shader source and the file at FragmentSourceFilePath as the fragment shader
 * source.
 *
 * @param VertexSourceFilePath Path to the vertex shader source file.
 * @param FragmentSourceFilePath Path to the fragment shader source file.
 * @return std::optional<std::pair<std::string, std::string>> An engaged optional
 * containing a pair `{vertexSource, fragmentSource}` when both files were opened
 * and read successfully; `std::nullopt` if either file could not be opened.
 */
std::optional<std::pair<std::string, std::string>>
ParseShaderFiles(const std::string& VertexSourceFilePath, const std::string& FragmentSourceFilePath)
{
    std::ifstream vertex(VertexSourceFilePath); // vertex contents
    std::ifstream fragment(FragmentSourceFilePath); // fragment contents

    if (!fragment.is_open() || !vertex.is_open()) {
        return std::nullopt;
    }

    std::stringstream frag_buffer;
    std::stringstream vert_buffer;


    frag_buffer << fragment.rdbuf();
    vert_buffer << vertex.rdbuf();

    return std::make_pair(vert_buffer.str(), frag_buffer.str());
}

/**
 * @brief Initializes the Shader by loading vertex and fragment source files, setting their sources, and compiling/linking the shader program.
 *
 * Attempts to read the provided vertex and fragment shader files; on success stores the file paths, applies the sources, and invokes shader compilation/linking via ResetShaders().
 * If either file fails to open or read, logs an error under the "SHADER" tag and leaves the Shader in an uninitialized state (constructor returns early).
 *
 * @param VertexSourceFilePath Path to the vertex shader source file.
 * @param FragmentSourceFilePath Path to the fragment shader source file.
 */
Shader::Shader(const std::string& VertexSourceFilePath, const std::string& FragmentSourceFilePath)
{
    auto result = ParseShaderFiles(VertexSourceFilePath, FragmentSourceFilePath);
    if(!result.has_value())
    {
        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("Couldn't retrieve fragment/vertex source files!");
        return;
    }

    auto ShaderSources = result.value();

    SetShaderSources(ShaderSources.first.c_str(), ShaderSources.second.c_str());
    VertexShaderSource = VertexSourceFilePath;
    FragmentShaderSource = FragmentSourceFilePath;

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

/**
 * @brief Reloads shader sources from the given file paths and rebuilds the shader program.
 *
 * Attempts to read the vertex and fragment shader source files at the provided paths; on
 * success the sources are stored, the stored file-path members are updated, and the shader
 * program is (re)compiled and linked. If file loading fails, an error is logged under the
 * "SHADER" tag and no changes are made to the current shader state.
 *
 * @param VertexSourceFilepath Path to the vertex shader source file.
 * @param FragmentSourceFilepath Path to the fragment shader source file.
 */
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
    VertexShaderSource = VertexSourceFilepath;
    FragmentShaderSource = FragmentSourceFilepath;

    ResetShaders(); // Forward to resetshaders, since it already handles all of the shader creation
}

/**
 * @brief Compile stored GLSL sources and create or replace the OpenGL shader program.
 *
 * Compiles the current vertex and fragment shader source strings, links them into a new
 * OpenGL program, and replaces the existing program handle. If shader sources are empty
 * the function returns without modifying state. Compilation or link failures are recorded
 * to the logger under the "SHADER" tag. Shader objects are deleted after linking.
 */
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

    // glBindAttribLocation(m_Program, 0, "position");
    // glBindAttribLocation(m_Program, 1, "color");
    // glBindAttribLocation(m_Program, 2, "texCoord");
    //
    // glBindAttribLocation(m_Program, 11, "row0"); // match your shader's names
    // glBindAttribLocation(m_Program, 12, "row1");
    // glBindAttribLocation(m_Program, 13, "row2");
    // glBindAttribLocation(m_Program, 14, "row3");
    //
    // glBindAttribLocation(m_Program, 15, "normalRow0");
    // glBindAttribLocation(m_Program, 16, "normalRow1");
    // glBindAttribLocation(m_Program, 17, "normalRow2");

    // Debug    
    int successA;
    int successB;
    int successC;
    char infoLogA[512];
    char infoLogB[512];
    char infoLogC[512];
    std::stringstream ss;
    glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &successA);
    glGetShaderiv(m_FragmentShader, GL_COMPILE_STATUS, &successB);
    glGetProgramiv(m_Program, GL_LINK_STATUS, &successC);
    
    if(!successA || !successB || !successC)
    {
        glGetShaderInfoLog(m_VertexShader, 512, NULL, infoLogA);
        glGetShaderInfoLog(m_FragmentShader, 512, NULL, infoLogB);
        glGetProgramInfoLog(m_Program, 512, NULL, infoLogC);
        
        ss << infoLogA << "\n";
        ss << infoLogB << "\n";
        ss << infoLogC;

        logger.AppendLogTag("SHADER", LogColors::CYAN);
        logger.LogError("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + ss.str());
    }

    glDeleteShader(m_VertexShader);
    glDeleteShader(m_FragmentShader);
}

/**
 * @brief Activates this shader program for use in the current OpenGL context.
 *
 * Dumps any accumulated logs before attempting to activate the program. If the stored program handle is not a valid OpenGL program, logs an error and does not change the current program.
 */
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


/**
 * @brief Retrieves the location of a named uniform in the shader program.
 *
 * @param name The uniform variable name to query.
 * @return int Location of the uniform, or `-1` if the shader program is not valid or the uniform is not found.
 */
int Shader::GetUniformLocation(const std::string& name)
{
    if (glIsProgram(m_Program))
        return glGetUniformLocation(m_Program, name.c_str());

    return -1;
}

/**
 * @brief Retrieve the stored vertex and fragment shader source file paths.
 *
 * @return std::pair<std::string, std::string> First: vertex shader file path; Second: fragment shader file path.
 */
std::pair<std::string, std::string> Shader::GetFilepaths() const
{
    return {VertexShaderSource, FragmentShaderSource};
}
