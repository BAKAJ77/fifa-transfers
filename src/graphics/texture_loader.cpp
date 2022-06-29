#include <graphics/texture_loader.h>
#include <util/directory_system.h>
#include <util/logging_system.h>

#include <glad/glad.h>
#include <stb_image.h>

void TextureLoader::LoadFromFile(const std::string_view& id, const std::string_view& fileName, bool flipOnLoad)
{
	// Skip loading the texture if it has already been loaded
	if (this->loadedTextures.find(id.data()) != this->loadedTextures.end())
	{
		LogSystem::GetInstance().OutputLog("Aborted texture loading process, there's already a loaded texture with the ID: " + 
			std::string(id), Severity::WARNING);
		return;
	}
	else
	{
		for (const auto& texture : this->loadedTextures)
		{
			if (texture.second.first == fileName)
			{
				LogSystem::GetInstance().OutputLog("Aborted texture loading process, the texture (" + std::string(fileName) + 
					") has already been loaded", Severity::WARNING);
				return;
			}
		}
	}

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

	this->loadedTextures[id.data()] = { fileName.data(),
		std::make_shared<TextureBuffer2D>(format, width, height, format, GL_UNSIGNED_BYTE, loadedPixelData, true) };
}

const TextureBuffer2DPtr TextureLoader::GetTexture(const std::string_view& id) const
{
	// Search through the unordered map for a texture matching the given ID
	auto iterator = this->loadedTextures.find(id.data());
	if (iterator != this->loadedTextures.end())
		return iterator->second.second;

	// No texture has been found a matching ID
	LogSystem::GetInstance().OutputLog("No loaded texture has the given ID: " + std::string(id), Severity::WARNING);
	return nullptr;
}

TextureLoader& TextureLoader::GetInstance()
{
	static TextureLoader instance;
	return instance;
}
