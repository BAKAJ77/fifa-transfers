#ifndef RENDERER_H
#define RENDERER_H

#include <core/window_frame.h>

#include <graphics/orthogonal_camera.h>
#include <graphics/shader_program.h>
#include <graphics/vertex_array.h>

#include <glm/glm.hpp>

class Renderer
{
private:
	WindowFramePtr appWindow;
	OrthogonalCamera viewport;
	ShaderProgramPtr geometryShader;
	VertexArrayPtr squareGeometryVao, triangleGeometryVao;

	glm::vec4 clearColor;
	float gamma;
private:
	Renderer();

	// Returns the generated model matrix based on the transformation data given.
	glm::mat4 GenerateModelMatrix(const glm::vec2& pos, const glm::vec2& size, float rotationAngle) const;
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
	void RenderSquare(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, float rotationAngle = 0.0f) const;

	// Renders a uni-colored triangle onto the screen.
	void RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotationAngle = 0.0f) const;

	// Renders a textured triangle onto the screen
	void RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, float rotationAngle = 0.0f) const;

	// Clears the post processing FBO and fills with the current set clear color.
	void Clear() const;

	// Returns singleton instance object of this class.
	static Renderer& GetInstance();
};

#endif
