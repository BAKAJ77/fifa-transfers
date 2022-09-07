#ifndef FONT_LOADER_H
#define FONT_LOADER_H

#include <graphics/buffer_objects.h>

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

struct FT_LibraryRec_;
using FT_Library = FT_LibraryRec_*;

struct GlyphData
{
	glm::vec2 bearing, size;
	uint32_t advanceX = 0, textureOffsetX = 0;
};

using GlyphMap = std::unordered_map<char, GlyphData>;

class Font
{
private:
	std::string fileName;

	TextureBuffer2DPtr bitmapTexture;
	GlyphMap glyphs;
	uint32_t resolution, styleIndex;
public:
	Font(FT_Library& lib, const std::string_view& fileName, uint32_t resolution, uint32_t styleIndex);
	~Font() = default;

	// Returns the file name of the loaded font.
	const std::string& GetFileName() const;

	// Returns the data of each glyph loaded from the font file.
	const GlyphMap& GetGlyphs() const;

	// Returns the generated bitmap texture loaded from the font file.
	const TextureBuffer2DPtr GetBitmap() const;

	// Returns the resolution of the the loaded glyphs in the font bitmap texture.
	const uint32_t& GetResolution() const;

	// Returns the index of the font style loaded.
	const uint32_t& GetStyleIndex() const;
};

using FontPtr = std::shared_ptr<Font>;

class FontLoader
{
private:
	FT_Library lib;
	std::unordered_map<std::string, FontPtr> loadedFonts;
private:
	FontLoader();
public:
	FontLoader(const FontLoader& other) = delete;
	FontLoader(FontLoader&& temp) noexcept = delete;
	~FontLoader();

	// Loads the font from the file specified. 
	// If the font being loaded is a variable font, you can use the 'styleIndex' parameter to select a specific font style to load.
	void LoadFromFile(const std::string_view& id, const std::string_view& fileName, uint32_t styleIndex = 0);

	// Frees the stored loaded font matching the ID given.
	void Free(const std::string_view& id);

	// Returns a shared pointer to the loaded font matching the ID given.
	// If no loaded font matches the ID given, nullptr will be returned instead.
	const FontPtr GetFont(const std::string_view& id) const;

	// Returns singleton instance object of this class.
	static FontLoader& GetInstance();
};

#endif