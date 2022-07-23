#ifndef TICK_BOX_H
#define TICK_BOX_H

#include <interface/button_base.h>
#include <graphics/font_loader.h>
#include <string>

class TickBox : public ButtonBase
{
private:
	FontPtr font;
	std::string labelText;
	glm::vec2 labelTextPosition;
	bool isTicked;
public:
	TickBox();
	TickBox(const glm::vec2& pos, const glm::vec2& buttonSize, const std::string_view& label, float opacity = 255.0f, float boxOffset = 15.0f,
		bool defaultValue = false);

	~TickBox() = default;

	// Resets the current 'isTicked' state to FALSE.
	void Reset();

	// Updates the tick box.
	void Update(const float& deltaTime, float animationSpeed) override;

	// Renders the tick box.
	void Render(float masterOpacity) const override;

	// Returns TRUE if the tick box is currently ticked off.
	bool isCurrentlyTicked() const;
};

#endif