#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <graphics/buffer_objects.h>
#include <unordered_map>
#include <string>

class TextureLoader
{
	using ImageTexture = std::pair<std::string, TextureBuffer2DPtr>; // [file name, texture object]
private:
	std::unordered_map<std::string, ImageTexture> loadedTextures;
private:
	TextureLoader() = default;
public:
	TextureLoader(const TextureLoader& other) = delete;
	TextureLoader(TextureLoader&& temp) noexcept = delete;
	~TextureLoader() = default;

	// Loads the texture from the file specified. 
	void LoadFromFile(const std::string_view& id, const std::string_view& fileName, bool flipOnLoad = false);

	// Returns a shared pointer to the loaded texture matching the ID given.
	// If no loaded texture matches the ID given, nullptr will be returned instead.
	const TextureBuffer2DPtr GetTexture(const std::string_view& id) const;

	// Returns singleton instance object of this class.
	static TextureLoader& GetInstance();
};

#endif