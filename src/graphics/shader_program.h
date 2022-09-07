#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <memory>
#include <unordered_map>
#include <string_view>
#include <glm/glm.hpp>

class ShaderProgram
{
private:
	enum class ShaderOperation
	{
		COMPILATION,
		LINKAGE
	};
private:
	uint32_t id;
	std::string vertexShaderPath, fragmentShaderPath;
	mutable std::unordered_map<std::string, uint32_t> uniformLocationCache;
private:
	// Validates that the specified shader operation was successful, if not the error log fetched will be outputted.
	void ValidateShaderOperation(const uint32_t& id, ShaderOperation operation) const;

	// Returns the location unit of the specified shader uniform variable.
	// The locations of fetched shader uniform are stored in the cache (unordered map) for faster future fetching.
	uint32_t GetUniformLocation(const std::string_view& uniformName) const;
public:
	ShaderProgram(const std::string_view& vshFileName, const std::string_view& fshFileName);
	~ShaderProgram();

	// Assigns a value to the shader uniform variable specified.
	void SetUniform(const std::string_view& uniformName, int value) const;
	void SetUniform(const std::string_view& uniformName, float value) const;
	void SetUniform(const std::string_view& uniformName, bool value) const;

	// Assigns a vector/matrix value to the shader uniform variable specified.
	void SetUniformGLM(const std::string_view& uniformName, const glm::vec2& vector) const;
	void SetUniformGLM(const std::string_view& uniformName, const glm::vec3& vector) const;
	void SetUniformGLM(const std::string_view& uniformName, const glm::vec4& vector) const;
	void SetUniformGLM(const std::string_view& uniformName, const glm::mat3& matrix) const;
	void SetUniformGLM(const std::string_view& uniformName, const glm::mat4& matrix) const;

	// Binds the shader program.
	void Bind() const;

	// Unbinds the shader program.
	void Unbind() const;

	// Returns the ID of the shader program.
	const uint32_t& GetID() const;

	// Returns the path to the vertex shader file.
	const std::string& GetVertexShaderPath() const;

	// Returns the path to the fragment shader file.
	const std::string& GetFragmentShaderPath() const;
};

using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;

namespace Memory
{
	// Returns a shared pointer to the newly created shader program.
	ShaderProgramPtr CreateShaderProgram(const std::string_view& vshFileName, const std::string_view& fshFileName);
}

#endif
