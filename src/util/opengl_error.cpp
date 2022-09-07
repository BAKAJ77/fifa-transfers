#include <util/opengl_error.h>
#include <util/logging_system.h>

#include <glad/glad.h>
#include <string>

void OpenGL::CheckError(const char* file, int line)
{
#if _DEBUG
	uint32_t errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string errorString;
		switch (errorCode)
		{
		case GL_INVALID_ENUM: errorString = "[OpenGL] -> The enum used is invalid"; break;
		case GL_INVALID_VALUE: errorString = "[OpenGL] -> The value given is invalid"; break;
		case GL_INVALID_OPERATION: errorString = "[OpenGL] -> The operation is invalid"; break;
		case GL_OUT_OF_MEMORY: errorString = "[OpenGL] -> Run out of free memory"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: errorString = "[OpenGL] -> The framebuffer operation is invalid"; break;
		}

		LogSystem::GetInstance().OutputLog(errorString + "(File: " + file + ", Line: " + std::to_string(line) + ")", Severity::FATAL);
	}
#endif
}
