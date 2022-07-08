#ifndef RADIO_BUTTONS_H
#define RADIO_BUTTONS_H

#include <interface/button_base.h>
#include <graphics/font_loader.h>
#include <vector>
#include <string>

class RadioButtonGroup
{
private:
	struct RadioButton
	{
		std::string id;
		glm::vec2 textSize;
		int value;

		ButtonBase button;
	};
private:
	FontPtr font;
	glm::vec2 position, buttonSize;
	std::vector<RadioButton> radioButtons;
	RadioButton* currentSelected;

	float currentOffset, opacity;
public:
	RadioButtonGroup();
	RadioButtonGroup(const glm::vec2& pos, const glm::vec2& buttonSize, float opacity = 255.0f);

	~RadioButtonGroup() = default;

	// Sets the opacity of the radio button group.
	void SetOpacity(float opacity);

	// Adds a new radio button into the group.
	void Add(const std::string_view& id, int value);

	// Resets the current selected to null.
	void Reset();

	// Updates the radio button group.
	void Update(const float& deltaTime);

	// Renders the radio button group.
	void Render(float masterOpacity) const;

	// Returns the value of the current selected radio button.
	// If no radio button is selected, an -1 is returned.
	int GetSelected() const;

	// Returns the opacity of the radio button group.
	const float& GetOpacity() const;
};

#endif