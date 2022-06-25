#include <graphics/renderer.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

void Renderer::Init(WindowFramePtr window)
{
	// Store application window shared pointer and enable blending (for transparency rendering)
	this->appWindow = window;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup the render scene viewport
	this->viewport = OrthogonalCamera({ 0.0f, 0.0f }, { 1920.0f, 1080.0f });

	// Load the required shaders
	this->geometryShader = Memory::CreateShaderProgram("geometry.glsl.vsh", "geometry.glsl.fsh");

	// Setup the geometry vertex array objects
	const float squareVertexData[] = { 
		-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 1.0f 
	};

	const float triangleVertexData[] = { 
		-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f 
	};

	const uint32_t squareIndexData[] = { 0, 1, 2, 0, 3, 2 };

	VertexBufferPtr squareGeometryVbo = Memory::CreateVertexBuffer(squareVertexData, sizeof(squareVertexData), GL_STATIC_DRAW);
	VertexBufferPtr triangleGeometryVbo = Memory::CreateVertexBuffer(triangleVertexData, sizeof(triangleVertexData), GL_STATIC_DRAW);
	IndexBufferPtr squareGeometryIbo = Memory::CreateIndexBuffer(squareIndexData, sizeof(squareIndexData), GL_STATIC_DRAW);

	this->squareGeometryVao = Memory::CreateVertexArray();
	this->squareGeometryVao->PushLayout(0, 2, 4 * sizeof(float));
	this->squareGeometryVao->PushLayout(1, 2, 4 * sizeof(float), 2 * sizeof(float));
	this->squareGeometryVao->AttachBuffers(squareGeometryVbo, squareGeometryIbo);

	this->triangleGeometryVao = Memory::CreateVertexArray();
	this->triangleGeometryVao->PushLayout(0, 2, 2 * sizeof(float));
	this->triangleGeometryVao->PushLayout(1, 2, 4 * sizeof(float), 2 * sizeof(float));
	this->triangleGeometryVao->AttachBuffers(triangleGeometryVbo);
}

glm::mat4 Renderer::GenerateModelMatrix(const glm::vec2& pos, const glm::vec2& size, float rotationAngle) const
{
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	return model;
}

void Renderer::SetClearColor(const glm::vec4& color)
{
	this->clearColor = color;
}

void Renderer::RenderSquare(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotationAngle) const
{
	// Bind the geometry shader and setup shader uniforms
	this->geometryShader->Bind();
	this->geometryShader->SetUniform("useDiffuseTexture", false);
	this->geometryShader->SetUniformGLM("diffuseColor", color / 255.0f);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the vertex array object
	this->squareGeometryVao->Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::RenderSquare(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, float rotationAngle) const
{
	// Bind the geometry shader and setup shader uniforms
	this->geometryShader->Bind();
	this->geometryShader->SetUniform("useDiffuseTexture", true);
	this->geometryShader->SetUniform("diffuseTexture", 0);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the texture buffer and vertex array
	texture->Bind(0);
	this->squareGeometryVao->Bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotationAngle) const
{
	// Bind the geometry shader and setup shader uniforms
	this->geometryShader->Bind();
	this->geometryShader->SetUniform("useDiffuseTexture", false);
	this->geometryShader->SetUniformGLM("diffuseColor", color / 255.0f);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the vertex array object
	this->triangleGeometryVao->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, float rotationAngle) const
{
	// Bind the geometry shader and setup shader uniforms
	this->geometryShader->Bind();
	this->geometryShader->SetUniform("useDiffuseTexture", true);
	this->geometryShader->SetUniform("diffuseTexture", 0);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the texture buffer and vertex array
	texture->Bind(0);
	this->triangleGeometryVao->Bind();

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::Clear() const
{
	glClearColor(clearColor.r / 255.0f, clearColor.g / 255.0f, clearColor.b / 255.0f, clearColor.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

Renderer& Renderer::GetInstance()
{
	static Renderer instance;
	return instance;
}
