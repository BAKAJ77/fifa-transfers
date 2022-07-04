#ifndef TEXT_INPUT_FIELD_H
#define TEXT_INPUT_FIELD_H

#include <graphics/font_loader.h>
#include <glm/glm.hpp>
#include <string>

class TextInputField
{
private:
	FontPtr textFont;

	std::string inputtedText;
	glm::vec2 position, size, textSize;
	float opacity, shadowDistance, fontSize;
public:
	TextInputField();
	TextInputField(const glm::vec2& pos, const glm::vec2& size, float opacity = 255.0f, float shadowDistance = 5.0f);
	
	~TextInputField();

	// Sets whether the text field is currently focused on or not.
	void SetFocus(bool focused);

	// Sets the position of the text field.
	void SetPosition(const glm::vec2& pos);

	// Sets the size of the text field.
	void SetSize(const glm::vec2& size);

	// Sets the opacity of the text field.
	void SetOpacity(float opacity);

	// Sets the shadow distance from the text field.
	void SetShadowDistance(float distance);

	// Clears all inputted characters in the text field.
	void Clear();

	// Updates the text field.
	void Update(const float& deltaTime);

	// Renders the text field.
	void Render(float masterOpacity) const;

	// Returns TRUE if the text field was clicked.
	bool WasClicked() const;
	
	// Returns TRUE if the text field is focused.
	bool IsFocused() const;

	// Returns the text inputted into the text field.
	const std::string& GetInputtedText() const;

	// Returns the position of the text field.
	const glm::vec2& GetPosition() const;

	// Returns the size of the text field.
	const glm::vec2& GetSize() const;

	// Returns the opacity of the button.
	const float& GetOpacity() const;

	// Returns the distance of the shadow from the button.
	const float& GetShadowDistance() const;
};

#endif