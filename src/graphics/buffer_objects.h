#ifndef BUFFER_OBJECTS_H
#define BUFFER_OBJECTS_H

#include <string_view>
#include <memory>

class VertexBuffer
{
private:
	uint32_t id;
public:
	VertexBuffer(const void* data, uint32_t size, uint32_t usage);
	~VertexBuffer();

	// Updates the data contained inside of the buffer with the new data given.
	void Update(const void* data, uint32_t size, uint32_t offset);

	// Binds the vertex buffer.
	void Bind() const;

	// Unbinds the vertex buffer.
	void Unbind() const;

	// Returns the ID of the vertex buffer.
	const uint32_t& GetID() const;
};

class IndexBuffer
{
private:
	uint32_t id;
public:
	IndexBuffer(const void* data, uint32_t size, uint32_t usage);
	~IndexBuffer();

	// Updates the data contained inside of the buffer with the new data given.
	void Update(const void* data, uint32_t size, uint32_t offset);

	// Binds the index buffer.
	void Bind() const;

	// Unbinds the index buffer.
	void Unbind() const;

	// Returns the ID of the index buffer.
	const uint32_t& GetID() const;
};

class TextureBuffer2D
{
private:
	uint32_t id, width, height;
private:
	// Configures default wrapping and filtering modes for the texture buffer.
	void SetDefaultModeSettings() const;
public:
	TextureBuffer2D(int internalFormat, uint32_t width, uint32_t height, uint32_t format, uint32_t type,
		const void* pixelData, bool genMipmaps);
	~TextureBuffer2D();

	// Sets the wrapping mode used for the texture buffer.
	void SetWrapMode(uint32_t sAxis, uint32_t tAxis);

	// Sets the filtering mode used for the texture buffer.
	void SetFilterMode(uint32_t min, uint32_t mag);

	// Updates the contents of the texture buffer at the specified offset with the pixel data given.
	void Update(int offsetX, int offsetY, uint32_t width, uint32_t height, uint32_t format, uint32_t type,
		const void* pixelData);

	// Binds the texture buffer.
	void Bind() const;

	// Binds the texture buffer.
	void Bind(uint32_t textureIndex) const;

	// Unbinds the texture buffer.
	void Unbind() const;

	// Returns the ID of the texture buffer.
	const uint32_t& GetID() const;

	// Returns the width of the texture buffer.
	const uint32_t& GetWidth() const;

	// Returns the height of the texture buffer.
	const uint32_t& GetHeight() const;
};

using VertexBufferPtr = std::shared_ptr<VertexBuffer>;
using IndexBufferPtr = std::shared_ptr<IndexBuffer>;
using TextureBuffer2DPtr = std::shared_ptr<TextureBuffer2D>;

namespace Memory
{
	// Returns shared pointer to the newly created vertex buffer.
	VertexBufferPtr CreateVertexBuffer(const void* data, uint32_t size, uint32_t usage);

	// Returns shared pointer to the newly created index buffer.
	IndexBufferPtr CreateIndexBuffer(const void* data, uint32_t size, uint32_t usage);

	// Returns a shared pointer to the newly created texture buffer.
	TextureBuffer2DPtr CreateTextureBuffer(int internalFormat, uint32_t width, uint32_t height, uint32_t format, uint32_t type, 
		const void* pixelData, bool genMipmaps = true);
}

#endif