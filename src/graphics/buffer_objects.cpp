#include <graphics/buffer_objects.h>
#include <util/directory_system.h>
#include <util/logging_system.h>
#include <util/opengl_error.h>

#include <glad/glad.h>
#include <stb_image.h>

//////////////////////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer(const void* data, uint32_t size, uint32_t usage)
{
    GLValidate(glGenBuffers(1, &this->id));
    GLValidate(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GLValidate(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
    GLValidate(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

VertexBuffer::~VertexBuffer()
{
    GLValidate(glDeleteBuffers(1, &this->id));
}

void VertexBuffer::Update(const void* data, uint32_t size, uint32_t offset)
{
    GLValidate(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GLValidate(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
    GLValidate(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::Bind() const
{
    GLValidate(glBindBuffer(GL_ARRAY_BUFFER, this->id));
}

void VertexBuffer::Unbind() const
{
    GLValidate(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

const uint32_t& VertexBuffer::GetID() const
{
    return this->id;
}

//////////////////////////////////////////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer(const void* data, uint32_t size, uint32_t usage)
{
    GLValidate(glGenBuffers(1, &this->id));
    GLValidate(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
    GLValidate(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage));
    GLValidate(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

IndexBuffer::~IndexBuffer()
{
    GLValidate(glDeleteBuffers(1, &this->id));
}

void IndexBuffer::Update(const void* data, uint32_t size, uint32_t offset)
{
    GLValidate(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
    GLValidate(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data));
    GLValidate(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::Bind() const
{
    GLValidate(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
}

void IndexBuffer::Unbind() const
{
    GLValidate(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

const uint32_t& IndexBuffer::GetID() const
{
    return this->id;
}

//////////////////////////////////////////////////////////////////////////////////////////////

TextureBuffer2D::TextureBuffer2D(int internalFormat, uint32_t width, uint32_t height, uint32_t format, uint32_t type, 
    const void* pixelData, bool genMipmaps) :
    width(width), height(height)
{
    // Generate and bind the texture buffer then configure wrap and filter modes
    GLValidate(glGenTextures(1, &this->id));
    GLValidate(glBindTexture(GL_TEXTURE_2D, this->id));

    this->SetDefaultModeSettings();

    // Fill the texture buffer with the pixel data given (then generate mipmaps if specified to do so)
    GLValidate(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixelData));
    GLValidate(glGenerateMipmap(GL_TEXTURE_2D));

    // Unbind the texture buffer
    GLValidate(glBindTexture(GL_TEXTURE_2D, 0));
}

TextureBuffer2D::~TextureBuffer2D()
{
    GLValidate(glDeleteTextures(1, &this->id));
}

void TextureBuffer2D::SetDefaultModeSettings() const
{
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

void TextureBuffer2D::SetWrapMode(uint32_t sAxis, uint32_t tAxis)
{
    GLValidate(glBindTexture(GL_TEXTURE_2D, this->id));
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sAxis));
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tAxis));
    GLValidate(glBindTexture(GL_TEXTURE_2D, 0));
}

void TextureBuffer2D::SetFilterMode(uint32_t min, uint32_t mag)
{
    GLValidate(glBindTexture(GL_TEXTURE_2D, this->id));
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min));
    GLValidate(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag));
    GLValidate(glBindTexture(GL_TEXTURE_2D, 0));
}

void TextureBuffer2D::Update(int offsetX, int offsetY, uint32_t width, uint32_t height, uint32_t format, uint32_t type,
    const void* pixelData)
{
    GLValidate(glBindTexture(GL_TEXTURE_2D, this->id));
    GLValidate(glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, format, type, pixelData));
    GLValidate(glBindTexture(GL_TEXTURE_2D, 0));
}

void TextureBuffer2D::Bind() const
{
    GLValidate(glBindTexture(GL_TEXTURE_2D, this->id));
}

void TextureBuffer2D::Bind(uint32_t textureIndex) const
{
    GLValidate(glActiveTexture(GL_TEXTURE0 + textureIndex));
    GLValidate(glBindTexture(GL_TEXTURE_2D, this->id));
}

void TextureBuffer2D::Unbind() const
{
    GLValidate(glBindTexture(GL_TEXTURE_2D, 0));
}

const uint32_t& TextureBuffer2D::GetID() const
{
    return this->id;
}

const uint32_t& TextureBuffer2D::GetWidth() const
{
    return this->width;
}

const uint32_t& TextureBuffer2D::GetHeight() const
{
    return this->height;
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

TextureBuffer2DPtr Memory::CreateTextureBuffer(int internalFormat, uint32_t width, uint32_t height, uint32_t format, uint32_t type, 
    const void* pixelData, bool genMipmaps)
{
    return std::make_shared<TextureBuffer2D>(internalFormat, width, height, format, type, pixelData, genMipmaps);
}