#include <graphics/renderer.h>
#include <serialization/config.h>
#include <util/opengl_error.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

Renderer::Renderer() :
	gamma(2.2f)
{}

void Renderer::Init(WindowFramePtr window)
{
	// Store application window shared pointer and enable blending (for transparency rendering)
	this->appWindow = window;

	GLValidate(glEnable(GL_BLEND));
	GLValidate(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// Setup the render scene viewport
	this->viewport = OrthogonalCamera({ 0.0f, 0.0f }, { 1920.0f, 1080.0f });

	// Load the gamma json config setting
	this->gamma = Serialization::GetConfigElement<float>("graphics", "gamma");

	// Load the required shaders
	this->geometryShader = Memory::CreateShaderProgram("geometry_render.glsl.vsh", "geometry_render.glsl.fsh");
	this->textShader = Memory::CreateShaderProgram("text_render.glsl.vsh", "text_render.glsl.fsh");
	
	// Setup the geometry vertex array objects
	const float squareVertexData[] = { 
		-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.0f, 1.0f, 0.5f,  0.5f, 1.0f, 1.0f
	};

	const float triangleVertexData[] = { 
		-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f 
	};

	this->squareGeometryVbo = Memory::CreateVertexBuffer(squareVertexData, sizeof(squareVertexData), GL_STATIC_DRAW);
	this->triangleGeometryVbo = Memory::CreateVertexBuffer(triangleVertexData, sizeof(triangleVertexData), GL_STATIC_DRAW);

	this->squareGeometryVao = Memory::CreateVertexArray();
	this->squareGeometryVao->PushLayout(0, 2, 4 * sizeof(float));
	this->squareGeometryVao->PushLayout(1, 2, 4 * sizeof(float), 2 * sizeof(float));
	this->squareGeometryVao->AttachBuffers(squareGeometryVbo);

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

Renderer::BatchedData Renderer::GenerateBatchedTextData(const FontPtr font, const std::string_view& text) const
{
	std::vector<float> vertexData;
	std::vector<uint32_t> indexData;

	glm::vec2 originPosition;
	uint32_t indexingOffset = 0;
	bool firstCharacter = false;

	// Allocate enough memory for each vector to store all the vertex and index data
	vertexData.reserve(text.size() * 16);
	indexData.reserve(text.size() * 6);

	for (const char& character : text)
	{
		// Generate the vertex coords
		const GlyphData& glyph = font->GetGlyphs().at(character);

		glm::vec2 topLeftVertex;
		firstCharacter ?
			topLeftVertex = { originPosition.x, originPosition.y - glyph.bearing.y } :
			topLeftVertex = { originPosition.x + glyph.bearing.x, originPosition.y - glyph.bearing.y };

		glm::vec2 topRightVertex = { topLeftVertex.x + glyph.size.x, topLeftVertex.y };
		glm::vec2 bottomLeftVertex = { topLeftVertex.x, topLeftVertex.y + glyph.size.y };
		glm::vec2 bottomRightVertex = { topRightVertex.x, bottomLeftVertex.y };

		// Generate the UV coords
		glm::vec2 topLeftUV = { glyph.textureOffsetX, 0.0f };
		glm::vec2 topRightUV = { glyph.textureOffsetX + glyph.size.x, 0.0f };
		glm::vec2 bottomLeftUV = { topLeftUV.x, glyph.size.y };
		glm::vec2 bottomRightUV = { topRightUV.x, glyph.size.y };

		// Normalize the UV coords, they must be within the bounds of -1.0f to 1.0f.
		topLeftUV.x /= (float)font->GetBitmap()->GetWidth();
		topRightUV.x /= (float)font->GetBitmap()->GetWidth();
		bottomLeftUV.x /= (float)font->GetBitmap()->GetWidth();
		bottomRightUV.x /= (float)font->GetBitmap()->GetWidth();

		topLeftUV.y /= (float)font->GetBitmap()->GetHeight();
		topRightUV.y /= (float)font->GetBitmap()->GetHeight();
		bottomLeftUV.y /= (float)font->GetBitmap()->GetHeight();
		bottomRightUV.y /= (float)font->GetBitmap()->GetHeight();

		// Push the generated vertex data into the vector array
		std::array<float, 16> generatedVertexData =
		{
			topLeftVertex.x, topLeftVertex.y, topLeftUV.x, topLeftUV.y,
			topRightVertex.x, topRightVertex.y, topRightUV.x, topRightUV.y,
			bottomLeftVertex.x, bottomLeftVertex.y, bottomLeftUV.x, bottomLeftUV.y,
			bottomRightVertex.x, bottomRightVertex.y, bottomRightUV.x, bottomRightUV.y
		};

		vertexData.insert(vertexData.end(), generatedVertexData.begin(), generatedVertexData.end());

		// Push the generated index data into the vector array
		std::array<uint32_t, 6> generatedIndexData =
		{
			0 + indexingOffset, 2 + indexingOffset, 3 + indexingOffset,
			0 + indexingOffset, 3 + indexingOffset, 1 + indexingOffset
		};

		indexData.insert(indexData.end(), generatedIndexData.begin(), generatedIndexData.end());

		// Update the necessary counters
		if (firstCharacter)
		{
			originPosition.x += glyph.advanceX - glyph.bearing.x;
			firstCharacter = false;
		}
		else
			originPosition.x += glyph.advanceX;

		indexingOffset += 4;
	}

	return { vertexData, indexData };
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
	this->geometryShader->SetUniform("gamma", this->gamma);

	this->geometryShader->SetUniformGLM("diffuseColor", color / 255.0f);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the vertex array object
	this->squareGeometryVao->Bind();
	GLValidate(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void Renderer::RenderSquare(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, const glm::vec4& colorMod, 
	float rotationAngle) const
{
	// Bind the geometry shader and setup shader uniforms
	this->geometryShader->Bind();
	this->geometryShader->SetUniform("useDiffuseTexture", true);
	this->geometryShader->SetUniform("gamma", this->gamma);

	this->geometryShader->SetUniform("diffuseTexture", 0);
	this->geometryShader->SetUniformGLM("diffuseColor", colorMod / 255.0f);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the texture buffer and vertex array
	texture->Bind(0);
	this->squareGeometryVao->Bind();

	GLValidate(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void Renderer::RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotationAngle) const
{
	// Bind the geometry shader and setup shader uniforms
	this->geometryShader->Bind();
	this->geometryShader->SetUniform("useDiffuseTexture", false);
	this->geometryShader->SetUniform("gamma", this->gamma);

	this->geometryShader->SetUniformGLM("diffuseColor", color / 255.0f);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the vertex array object
	this->triangleGeometryVao->Bind();
	GLValidate(glDrawArrays(GL_TRIANGLES, 0, 3));
}

void Renderer::RenderTriangle(const glm::vec2& pos, const glm::vec2& size, const TextureBuffer2DPtr texture, const glm::vec4& colorMod, 
	float rotationAngle) const
{
	// Bind the geometry shader and setup shader uniforms
	this->geometryShader->Bind();
	this->geometryShader->SetUniform("useDiffuseTexture", true);
	this->geometryShader->SetUniform("gamma", this->gamma);

	this->geometryShader->SetUniform("diffuseTexture", 0);
	this->geometryShader->SetUniformGLM("diffuseColor", colorMod / 255.0f);
	this->geometryShader->SetUniformGLM("modelMatrix", this->GenerateModelMatrix(pos, size, rotationAngle));
	this->geometryShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());

	// Bind the texture buffer and vertex array
	texture->Bind(0);
	this->triangleGeometryVao->Bind();

	GLValidate(glDrawArrays(GL_TRIANGLES, 0, 3));
}

void Renderer::RenderText(const glm::vec2& pos, const glm::vec4& color, const FontPtr font, uint32_t fontSize, 
	const std::string_view& text, float rotationAngle) const
{
	// Generate the batched vertex and index data
	const BatchedData renderData = this->GenerateBatchedTextData(font, text);

	// Create and setup the buffers and vertex array
	VertexBufferPtr textVbo = Memory::CreateVertexBuffer(renderData.first.data(),
		(uint32_t)renderData.first.size() * sizeof(float), GL_STATIC_DRAW);

	IndexBufferPtr textIbo = Memory::CreateIndexBuffer(renderData.second.data(),
		(uint32_t)renderData.second.size() * sizeof(uint32_t), GL_STATIC_DRAW);

	VertexArrayPtr textVao = Memory::CreateVertexArray();
	textVao->PushLayout(0, 2, 4 * sizeof(float));
	textVao->PushLayout(1, 2, 4 * sizeof(float), 2 * sizeof(float));
	textVao->AttachBuffers(textVbo, textIbo);

	// Generate the model matrix
	const glm::mat4 modelMatrix = this->GenerateModelMatrix(pos, glm::vec2((float)fontSize / (float)font->GetResolution()),
		rotationAngle);

	// Bind the shader and setup the shader uniforms
	this->textShader->Bind();

	this->textShader->SetUniform("fontBitmap", 0);
	this->textShader->SetUniformGLM("cameraMatrix", this->viewport.GetMatrix());
	this->textShader->SetUniformGLM("modelMatrix", modelMatrix);
	this->textShader->SetUniformGLM("diffuseColor", color / 255.0f);

	// Bind the vertex array and font bitmap texture
	textVao->Bind();
	font->GetBitmap()->Bind(0);

	GLValidate(glDrawElements(GL_TRIANGLES, (uint32_t)renderData.second.size(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Clear() const
{
	GLValidate(glClearColor(clearColor.r / 255.0f, clearColor.g / 255.0f, clearColor.b / 255.0f, clearColor.a / 255.0f));
	GLValidate(glClear(GL_COLOR_BUFFER_BIT));
}

glm::vec2 Renderer::GetTextSize(const FontPtr font, uint32_t fontSize, const std::string_view& text) const
{
	glm::vec2 totalSize;
	float minY = 0.0f, maxY = 0.0f;
	bool firstCharacter = true;

	for (uint32_t i = 0; i < text.size(); i++)
	{
		const GlyphData& glyphMetrics = font->GetGlyphs().at(text[i]);

		// WIDTH
		if (firstCharacter)
		{
			totalSize.x += glyphMetrics.advanceX;
			firstCharacter = false;
		}
		else if (i == text.size() - 1)
			totalSize.x += glyphMetrics.bearing.x + glyphMetrics.size.x;
		else
			totalSize.x += glyphMetrics.bearing.x + glyphMetrics.advanceX;

		// MAX POINT
		if (glyphMetrics.bearing.y > maxY)
			maxY = glyphMetrics.bearing.y;

		// MIN POINT
		if (glyphMetrics.bearing.y - glyphMetrics.size.y < minY)
			minY = glyphMetrics.bearing.y - glyphMetrics.size.y;
	}

	totalSize.y = maxY - minY;
	totalSize *= glm::vec2(static_cast<float>(fontSize) / static_cast<float>(font->GetResolution()));

	return totalSize;
}

Renderer& Renderer::GetInstance()
{
	static Renderer instance;
	return instance;
}
