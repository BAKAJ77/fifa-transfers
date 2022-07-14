#ifndef DROP_DOWN_H
#define DROP_DOWN_H

#include <interface/button_base.h>
#include <graphics/font_loader.h>
#include <vector>

class DropDown
{
private:
	struct Element
	{
		std::string id;
		ButtonBase button;
		glm::vec2 textSize;

		int value;
	};
private:
	FontPtr font;
	glm::vec2 position, size;
	float fontSize, opacity;
	bool doDropDown, clicked, released;

	int selectionsOffset, maxSelectionsVisible;
	std::vector<Element> selections;
	Element currentSelected;
public:
	DropDown();
	DropDown(const glm::vec2& pos, const glm::vec2& size, float opacity = 255.0f);
	DropDown(const DropDown& other) = default;
	DropDown(DropDown&& temp) noexcept;

	~DropDown() = default;

	DropDown& operator=(const DropDown& other) = default;
	DropDown& operator=(DropDown&& temp) noexcept;
	
	// Sets the opacity of the drop down.
	void SetOpacity(float opacity);

	// Adds a selection option element to the drop down.
	void AddSelection(const std::string_view& id, int value);

	// Clears all the selection option elements.
	void Clear();

	// Resets the selected option to empty.
	void Reset();

	// Updates the drop down.
	void Update(const float& deltaTime);

	// Render the drop down.
	void Render(float masterOpacity) const;

	// Returns the value of the current selected option element.
	int GetCurrentSelected() const;

	// Returns TRUE if the drop down is active i.e the selection options have dropped down.
	bool IsDroppedDown() const;

	// Returns the position of the drop down.
	const glm::vec2& GetPosition() const;

	// Returns the size of the drop down.
	const glm::vec2& GetSize() const;

	// Returns the opacity of the drop down.
	const float& GetOpacity() const;
};

#endif
