#ifndef BUFFER_OBJECTS_H
#define BUFFER_OBJECTS_H

#include <stdint.h>
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

using VertexBufferPtr = std::shared_ptr<VertexBuffer>;
using IndexBufferPtr = std::shared_ptr<IndexBuffer>;

namespace Memory
{
	// Returns shared pointer to the newly created vertex buffer.
	VertexBufferPtr CreateVertexBuffer(const void* data, uint32_t size, uint32_t usage);

	// Returns shared pointer to the newly created index buffer.
	IndexBufferPtr CreateIndexBuffer(const void* data, uint32_t size, uint32_t usage);
}

#endif