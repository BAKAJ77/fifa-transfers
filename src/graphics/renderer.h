#ifndef RENDERER_H
#define RENDERER_H

#include <core/window_frame.h>

#include <graphics/orthogonal_camera.h>
#include <graphics/font_loader.h>
#include <graphics/shader_program.h>
#include <graphics/vertex_array.h>

#include <glm/glm.hpp>

class Renderer
{
private:
	using BatchedData = std::pair<std::vector<float>, std::vector<uint32_t>>;
private:
	WindowFramePtr appWindow;
	OrthogonalCamera viewport;
	ShaderProgramPtr geometryShader, textShader;

	VertexBufferPtr squareGeometryVbo, triangleGeometryVbo;
	VertexArrayPtr squareGeometryVao, triangleGeometryVao;

	glm::vec4 clearColor;
	float gamma;
private:
	Renderer();

	// Returns the generated model matrix based on the transformation data given.
	glm::mat4 GenerateModelMatrix(const glm::vec2& pos, const glm::vec2& size, float rotationAngle) const;

	// Returns a pair of vectors, one containing vertex data and the other containing index data.
	// The vertex and index data inside the vectors are the result of all the glyphs in the text given having their
	// vertex and index data all batched into their respective vector containers.
	BatchedData GenerateBatchedTextData(const FontPtr font, const std::string_view& text) const;
public:
	Renderer(const Renderer& other) = delete;
	Renderer(Renderer&& other) noexcept = delete;
	~Renderer() = default;

	// Initializes the renderer.
	void Init(WindowFramePtr window);

	// Sets the renderer clear color.
	void SetClearColor(const glm::vec4& color);

	// Renders a uni-colored square onto the screen.
	void RenderSquare(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotationAngle = 0.0f) const;

	// Renders a textured square onto the screen.
	void RenderSquare(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, 
		const glm::vec4& colorMod = glm::vec4(255.0f), float rotationAngle = 0.0f) const;

	// Renders a uni-colored triangle onto the screen.
	void RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotationAngle = 0.0f) const;

	// Renders a textured triangle onto the screen
	void RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, 
		const glm::vec4& colorMod = glm::vec4(255.0f), float rotationAngle = 0.0f) const;

	// Renders a uni-colored string of text onto the screen.
	void RenderText(const glm::vec2& pos, const glm::vec4& color, const FontPtr font, uint32_t fontSize, const std::string_view& text, 
		float rotationAngle = 0.0f) const;

	// Clears the post processing FBO and fills with the current set clear color.
	void Clear() const;

	// Returns the expected size of the given text string when rendered.
	glm::vec2 GetTextSize(const FontPtr font, uint32_t fontSize, const std::string_view& text) const;

	// Returns singleton instance object of this class.
	static Renderer& GetInstance();
};

#endif
