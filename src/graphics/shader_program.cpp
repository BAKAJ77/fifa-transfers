#include <graphics/shader_program.h>
#include <util/directory_system.h>
#include <util/logging_system.h>
#include <util/opengl_error.h>

#include <fstream>
#include <sstream>
#include <glad/glad.h>

ShaderProgram::ShaderProgram(const std::string_view& vshFileName, const std::string_view& fshFileName) :
	vertexShaderPath(Util::GetAppDataDirectory() + "shaders/" + vshFileName.data()), 
	fragmentShaderPath(Util::GetAppDataDirectory() + "shaders/" + fshFileName.data())
{
	// Load the shader code from both shader files
	std::ifstream vertexFileStream, fragmentFileStream;
	
	vertexFileStream.open(this->vertexShaderPath);
	if (vertexFileStream.fail())
		LogSystem::GetInstance().OutputLog("Failed to open the vertex shader file at path: " + this->vertexShaderPath, Severity::FATAL);

	fragmentFileStream.open(this->fragmentShaderPath);
	if (fragmentFileStream.fail())
		LogSystem::GetInstance().OutputLog("Failed to open the fragment shader file at path" + this->fragmentShaderPath, Severity::FATAL);

	std::stringstream vshContentsStream, fshContentsStream;
	vshContentsStream << vertexFileStream.rdbuf();
	fshContentsStream << fragmentFileStream.rdbuf();

	const std::string vshSrcCodeStr = vshContentsStream.str(), fshSrcCodeStr = fshContentsStream.str();
	const char* vshSrcCode = vshSrcCodeStr.c_str();
	const char* fshSrcCode = fshSrcCodeStr.c_str();

	// Compile the loaded shader code
	const uint32_t vshID = GLValidate(glCreateShader(GL_VERTEX_SHADER));
	GLValidate(glShaderSource(vshID, 1, &vshSrcCode, nullptr));
	GLValidate(glCompileShader(vshID));

	this->ValidateShaderOperation(vshID, ShaderOperation::COMPILATION);

	const uint32_t fshID = GLValidate(glCreateShader(GL_FRAGMENT_SHADER));
	GLValidate(glShaderSource(fshID, 1, &fshSrcCode, nullptr));
	GLValidate(glCompileShader(fshID));

	this->ValidateShaderOperation(fshID, ShaderOperation::COMPILATION);

	// Create the shader program, then attach the compiled shaders and perform linking operation
	this->id = GLValidate(glCreateProgram());
	GLValidate(glAttachShader(this->id, vshID));
	GLValidate(glAttachShader(this->id, fshID));
	GLValidate(glLinkProgram(this->id));

	this->ValidateShaderOperation(this->id, ShaderOperation::LINKAGE);

	GLValidate(glDeleteShader(vshID));
	GLValidate(glDeleteShader(fshID));
}

ShaderProgram::~ShaderProgram()
{
	GLValidate(glDeleteProgram(this->id));
}

void ShaderProgram::ValidateShaderOperation(const uint32_t& id, ShaderOperation operation) const
{
	std::unique_ptr<char> errorLog;
	int logLength = 0;

	// Get the length of the pending error log
	// If there is a pending error log (the length fetched is larger than 0) then fetch the error log string and store it
	switch (operation)
	{
	case ShaderOperation::COMPILATION:
		GLValidate(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength));
		if (logLength > 0)
		{
			errorLog = std::unique_ptr<char>(new char[logLength]);
			GLValidate(glGetShaderInfoLog(id, logLength, nullptr, errorLog.get()));
		}
		break;
	case ShaderOperation::LINKAGE:
		GLValidate(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength));
		if (logLength > 0)
		{
			errorLog = std::unique_ptr<char>(new char[logLength]);
			GLValidate(glGetProgramInfoLog(id, logLength, nullptr, errorLog.get()));
		}
		break;
	}

	// If an error log was fetched then output it
	if (logLength > 0)
		LogSystem::GetInstance().OutputLog(errorLog.get(), Severity::FATAL);
}

uint32_t ShaderProgram::GetUniformLocation(const std::string_view& uniformName) const
{
	auto cacheIterator = this->uniformLocationCache.find(uniformName.data()); // Check if location is already cached
	if (cacheIterator == this->uniformLocationCache.end())
	{
		// It is not cached so fetch it from GPU, store it in cache and return it
		const uint32_t location = GLValidate(glGetUniformLocation(this->id, uniformName.data()));
		this->uniformLocationCache[uniformName.data()] = location;

		return location;
	}

	return cacheIterator->second; // The location was found in cache
}

void ShaderProgram::SetUniform(const std::string_view& uniformName, int value) const
{
	GLValidate(glUniform1i(this->GetUniformLocation(uniformName), value));
}

void ShaderProgram::SetUniform(const std::string_view& uniformName, float value) const
{
	GLValidate(glUniform1f(this->GetUniformLocation(uniformName), value));
}

void ShaderProgram::SetUniform(const std::string_view& uniformName, bool value) const
{
	GLValidate(glUniform1i(this->GetUniformLocation(uniformName), (int)value));
}

void ShaderProgram::SetUniformGLM(const std::string_view& uniformName, const glm::vec2& vector) const
{
	GLValidate(glUniform2fv(this->GetUniformLocation(uniformName), 1, &vector[0]));
}

void ShaderProgram::SetUniformGLM(const std::string_view& uniformName, const glm::vec3& vector) const
{
	GLValidate(glUniform3fv(this->GetUniformLocation(uniformName), 1, &vector[0]));
}

void ShaderProgram::SetUniformGLM(const std::string_view& uniformName, const glm::vec4& vector) const
{
	GLValidate(glUniform4fv(this->GetUniformLocation(uniformName), 1, &vector[0]));
}

void ShaderProgram::SetUniformGLM(const std::string_view& uniformName, const glm::mat3& matrix) const
{
	GLValidate(glUniformMatrix3fv(this->GetUniformLocation(uniformName), 1, false, &matrix[0][0]));
}

void ShaderProgram::SetUniformGLM(const std::string_view& uniformName, const glm::mat4& matrix) const
{
	GLValidate(glUniformMatrix4fv(this->GetUniformLocation(uniformName), 1, false, &matrix[0][0]));
}

void ShaderProgram::Bind() const
{
	GLValidate(glUseProgram(this->id));
}

void ShaderProgram::Unbind() const
{
	GLValidate(glUseProgram(0));
}

const uint32_t& ShaderProgram::GetID() const
{
	return this->id;
}

const std::string& ShaderProgram::GetVertexShaderPath() const
{
	return this->vertexShaderPath;
}

const std::string& ShaderProgram::GetFragmentShaderPath() const
{
	return this->fragmentShaderPath;
}

ShaderProgramPtr Memory::CreateShaderProgram(const std::string_view& vshFileName, const std::string_view& fshFileName)
{
	return std::make_shared<ShaderProgram>(vshFileName, fshFileName);
}
