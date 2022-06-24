#include <graphics/buffer_objects.h>
#include <glad/glad.h>

//////////////////////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer(const void* data, uint32_t size, uint32_t usage)
{
    glGenBuffers(1, &this->id);
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &this->id);
}

void VertexBuffer::Update(const void* data, uint32_t size, uint32_t offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

const uint32_t& VertexBuffer::GetID() const
{
    return this->id;
}

//////////////////////////////////////////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer(const void* data, uint32_t size, uint32_t usage)
{
    glGenBuffers(1, &this->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &this->id);
}

void IndexBuffer::Update(const void* data, uint32_t size, uint32_t offset)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

const uint32_t& IndexBuffer::GetID() const
{
    return this->id;
}

//////////////////////////////////////////////////////////////////////////////////////////////

VertexBufferPtr Memory::CreateVertexBuffer(const void* data, uint32_t size, uint32_t usage)
{
    return std::make_shared<VertexBuffer>(data, size, usage);
}

IndexBufferPtr Memory::CreateIndexBuffer(const void* data, uint32_t size, uint32_t usage)
{
    return std::make_shared<IndexBuffer>(data, size, usage);
}
