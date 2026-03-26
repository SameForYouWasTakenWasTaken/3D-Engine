#include <iostream>
#include <spdlog/spdlog.h>

#include "Debug.hpp"
#include "../../../vendor/Logger.hpp"
#include <GLFW/glfw3.h>

//https://learnopengl.com/In-Practice/Debugging

void GLAPIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

	spdlog::info("---------------");
	spdlog::info("Debug message ({}): {}", id, message);

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             spdlog::info("Source: API"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   spdlog::info("Source: Window System"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: spdlog::info("Source: Shader Compiler"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     spdlog::info("Source: Third Party"); break;
		case GL_DEBUG_SOURCE_APPLICATION:     spdlog::info("Source: Application"); break;
		case GL_DEBUG_SOURCE_OTHER:           spdlog::info("Source: Other"); break;
	}

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               spdlog::info("Type: Error"); break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: spdlog::info("Type: Deprecated Behaviour"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  spdlog::info("Type: Undefined Behaviour"); break; 
		case GL_DEBUG_TYPE_PORTABILITY:         spdlog::info("Type: Portability"); break;
		case GL_DEBUG_TYPE_PERFORMANCE:         spdlog::info("Type: Performance"); break;
		case GL_DEBUG_TYPE_MARKER:              spdlog::info("Type: Marker"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          spdlog::info("Type: Push Group"); break;
		case GL_DEBUG_TYPE_POP_GROUP:           spdlog::info("Type: Pop Group"); break;
		case GL_DEBUG_TYPE_OTHER:               spdlog::info("Type: Other"); break;
	}
	
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         spdlog::error("Severity: high"); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       spdlog::warn("Severity: medium"); break;
		case GL_DEBUG_SEVERITY_LOW:          spdlog::warn("Severity: low"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: spdlog::info("Severity: notification"); break;
	}
	spdlog::info("");
}

void enableReportGlErrors()
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	glDebugMessageCallback(0, nullptr);
}