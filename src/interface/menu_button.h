#ifndef MENU_BUTTON_H
#define MENU_BUTTON_H

#include <interface/button_base.h>
#include <graphics/font_loader.h>
#include <core/audio_system.h>

class MenuButton : public ButtonBase
{
private:
	FontPtr textFont;
	AudioPtr sfx;

	std::string text;
	glm::vec2 textSize;
	float currentFontSize, baseFontSize;
	bool sfxPlayed;
public:
	MenuButton();
	MenuButton(const glm::vec2& pos, const glm::vec2& baseSize, const glm::vec2& maxSize, const std::string_view& text, 
		float opacity = 255.0f);

	~MenuButton() = default;

	// Updates the button state.
	virtual void Update(const float& deltaTime, float animationSpeed) override;

	// Renders the button.
	virtual void Render(float masterOpacity) const override;

	// Returns the button text.
	const std::string& GetText() const;
};

#endif