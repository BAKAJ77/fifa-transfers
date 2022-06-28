#include <graphics/vertex_array.h>
#include <util/opengl_error.h>

#include <glad/glad.h>
#include <cassert>

VertexArray::VertexArray()
{
    GLValidate(glGenVertexArrays(1, &this->id));
}

VertexArray::~VertexArray()
{
    GLValidate(glDeleteVertexArrays(1, &this->id));
}

void VertexArray::PushLayout(uint32_t index, uint32_t size, uint32_t stride, uint32_t offset, uint32_t divisor, bool normalized)
{
    this->layouts.push_back({ index, size, GL_FLOAT, stride, offset, divisor, normalized });
}

void VertexArray::AttachBuffers(VertexBufferPtr vertexBuffer, IndexBufferPtr indexBuffer)
{
    assert(vertexBuffer != nullptr);

    // Bind the vertex array, then bind the vertex buffer (and index buffer if given one)
    GLValidate(glBindVertexArray(this->id));

    vertexBuffer->Bind();
    if (indexBuffer)
        indexBuffer->Bind();

    // Configure the vertex attribute pointers using the stored vertex layouts
    for (const VertexLayout& layout : this->layouts)
    {
        GLValidate(glEnableVertexAttribArray(layout.index));
        GLValidate(glVertexAttribPointer(layout.index, layout.size, layout.type, layout.normalized, layout.stride, 
            (void*)((uint64_t)layout.offset)));
        GLValidate(glVertexAttribDivisor(layout.index, layout.divisor));
    }

    this->layouts.clear();

    // Finally unbind all objects
    GLValidate(glBindVertexArray(0));

    vertexBuffer->Unbind();
    if (indexBuffer)
        indexBuffer->Unbind();
}

void VertexArray::Bind() const
{
    GLValidate(glBindVertexArray(this->id));
}

void VertexArray::Unbind() const
{
    GLValidate(glBindVertexArray(0));
}

const uint32_t& VertexArray::GetID() const
{
    return this->id;
}

VertexArrayPtr Memory::CreateVertexArray()
{
    return std::make_shared<VertexArray>();
}
