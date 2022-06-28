#include <graphics/ttf_font_loader.h>
#include <serialization/config.h>
#include <util/logging_system.h>
#include <util/directory_system.h>
#include <util/opengl_error.h>

#include <algorithm>
#include <glad/glad.h>
#include <ft2build.h>

#include FT_FREETYPE_H

Font::Font(const std::string_view& fileName, uint32_t resolution) :
	resolution(std::clamp(resolution, (uint32_t)30, (uint32_t)230))
{
	// Initialize the freetype library
	FT_Library freeTypeLib = nullptr;
	if (FT_Init_FreeType(&freeTypeLib))
		LogSystem::GetInstance().OutputLog("Failed to initialize freetype", Severity::FATAL);

	// Fetch the game asset directory to construct path to font file
	const std::string fontPath = Util::GetAppDataDirectory() + "fonts/" + fileName.data();

	// Load the font face
	FT_Face fontFace;
	if (FT_New_Face(freeTypeLib, fontPath.c_str(), 0, &fontFace))
		LogSystem::GetInstance().OutputLog("Failed to load the font: " + fontPath, Severity::FATAL);

	FT_Set_Pixel_Sizes(fontFace, 0, this->resolution);

	// Retrieve the metric data of each glyph in the font
	// Also, figure out how large the texture will need to be to store every glyph bitmap in it
	uint32_t totalBitmapWidth = 0, totalBitmapHeight = 0;
	for (char glyph = 0; glyph != 127; glyph++)
	{
		if (FT_Load_Char(fontFace, glyph, FT_LOAD_BITMAP_METRICS_ONLY))
			LogSystem::GetInstance().OutputLog("Failed to fetch glyph metrics", Severity::FATAL);

		// Store the character's glyph metrics
		GlyphData glyphMetrics;
		glyphMetrics.bearing = { fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top };
		glyphMetrics.size = { fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows };
		glyphMetrics.advanceX = (fontFace->glyph->advance.x >> 6);
		glyphMetrics.textureOffsetX = totalBitmapWidth;

		this->glyphs[glyph] = glyphMetrics;

		// Update the total bitmap width and height counters
		constexpr uint32_t glyphBitmapOffset = 10;
		totalBitmapWidth += (uint32_t)glyphMetrics.size.x + glyphBitmapOffset;

		if (totalBitmapHeight < glyphMetrics.size.y)
			totalBitmapHeight = (uint32_t)glyphMetrics.size.y;
	}

	// Allocate an empty texture buffer consisting of enough space to store all glypth bitmaps
	this->bitmapTexture = Memory::CreateTextureBuffer(GL_RED, totalBitmapWidth, totalBitmapHeight, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	// Fill the texture buffer with the loaded glyph bitmaps
	this->bitmapTexture->Bind();
	GLValidate(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	for (char glyph = 0; glyph != 127; glyph++)
	{
		if (FT_Load_Char(fontFace, glyph, FT_LOAD_RENDER))
			LogSystem::GetInstance().OutputLog("Failed to fetch glyph bitmap", Severity::FATAL);

		const GlyphData& glyphMetrics = this->glyphs[glyph];
		this->bitmapTexture->Update(glyphMetrics.textureOffsetX, 0, (uint32_t)glyphMetrics.size.x, (uint32_t)glyphMetrics.size.y,
			GL_RED, GL_UNSIGNED_BYTE, fontFace->glyph->bitmap.buffer);
	}

	this->bitmapTexture->Unbind();
	this->bitmapTexture->SetWrapMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
}

const std::string& Font::GetFileName() const
{
	return this->fileName;
}

const GlyphMap& Font::GetGlyphs() const
{
	return this->glyphs;
}

const TextureBuffer2DPtr Font::GetBitmap() const
{
	return this->bitmapTexture;
}

const uint32_t& Font::GetResolution() const
{
	return this->resolution;
}

FontPtr Memory::LoadFontFromFile(const std::string_view& fileName)
{
	const uint32_t resolution = Serialization::GetConfigElement<uint32_t>("graphics", "textQuality");
	return std::make_shared<Font>(fileName, resolution);
}
