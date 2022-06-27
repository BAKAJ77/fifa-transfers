#include <graphics/buffer_objects.h>
#include <util/directory_system.h>
#include <util/logging_system.h>

#include <glad/glad.h>
#include <stb_image.h>

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

TextureBuffer2D::TextureBuffer2D(int internalFormat, uint32_t width, uint32_t height, uint32_t format, uint32_t type, 
    const void* pixelData, bool genMipmaps) :
    width(width), height(height)
{
    // Generate and bind the texture buffer then configure wrap and filter modes
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    this->SetDefaultModeSettings();

    // Fill the texture buffer with the pixel data given (then generate mipmaps if specified to do so)
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture buffer
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureBuffer2D::~TextureBuffer2D()
{
    glDeleteTextures(1, &this->id);
}

void TextureBuffer2D::SetDefaultModeSettings() const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TextureBuffer2D::SetWrapMode(uint32_t sAxis, uint32_t tAxis)
{
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sAxis);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tAxis);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureBuffer2D::SetFilterMode(uint32_t min, uint32_t mag)
{
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureBuffer2D::Update(int offsetX, int offsetY, uint32_t width, uint32_t height, uint32_t format, uint32_t type,
    const void* pixelData)
{
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, format, type, pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureBuffer2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->id);
}

void TextureBuffer2D::Bind(uint32_t textureIndex) const
{
    glActiveTexture(GL_TEXTURE0 + textureIndex);
    glBindTexture(GL_TEXTURE_2D, this->id);
}

void TextureBuffer2D::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
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

TextureBuffer2DPtr Memory::LoadImageFromFile(const std::string_view& fileName, bool flipOnLoad)
{
    // Load the image file using STB
    int width = 0, height = 0, channels = 0;
    const std::string filePath = Util::GetAppDataDirectory() + "textures/" + fileName.data();

    stbi_set_flip_vertically_on_load(flipOnLoad);
    const uint8_t* loadedPixelData = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (!loadedPixelData)
        LogSystem::GetInstance().OutputLog("Failed to load image file at path: " + filePath, Severity::WARNING);

    // Deduce the correct format enum to use
    uint32_t format = 0x0;
    channels > 3 ? format = GL_RGBA : format = GL_RGB;

    return std::make_shared<TextureBuffer2D>(format, width, height, format, GL_UNSIGNED_BYTE, loadedPixelData, true);
}