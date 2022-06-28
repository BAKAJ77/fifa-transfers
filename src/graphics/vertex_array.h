#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <graphics/buffer_objects.h>
#include <vector>

class VertexArray
{
private:
	struct VertexLayout
	{
		uint32_t index, size, type, stride, offset, divisor;
		bool normalized;
	};
private:
	uint32_t id;
	std::vector<VertexLayout> layouts;
public:
	VertexArray();
	~VertexArray();

	// Pushes a new vertex layout.
	// This defines the layout of vertex data in the vertex buffer given from the next AttachBuffers() call. 
	void PushLayout(uint32_t index, uint32_t size, uint32_t stride, uint32_t offset = 0, uint32_t divisor = 0, bool normalized = false);

	// Attaches the vertex and index buffer to the vertex array object.
	void AttachBuffers(VertexBufferPtr vertexBuffer, IndexBufferPtr indexBuffer = nullptr);

	// Binds the vertex array object.
	void Bind() const;

	// Unbinds the vertex array object.
	void Unbind() const;

	// Returns the ID of the vertex array object.
	const uint32_t& GetID() const;
};

using VertexArrayPtr = std::shared_ptr<VertexArray>;

namespace Memory
{
	// Returns a shared pointer to the newly created vertex array.
	VertexArrayPtr CreateVertexArray();
}

#endif