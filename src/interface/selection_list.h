#ifndef SELECTION_LIST_H
#define SELECTION_LIST_H

#include <interface/button_base.h>
#include <graphics/font_loader.h>
#include <core/audio_system.h>

#include <vector>

class SelectionList
{
private:
	struct Category
	{
		std::string text;
		glm::vec2 textSize;
	};
public:
	struct Element
	{
		std::vector<Category> categoryValues;
		ButtonBase button;
		int value;
	};
private:
	FontPtr font;
	glm::vec2 position, size;
	float fontSize, buttonHeight, opacity;
	int listOffset, maxListSelectionsVisible;

	std::vector<Category> listCategories;
	std::vector<Element> listElements;
	Element* currentSelected;

	ButtonBase nextPageButton, previousPageButton;
public:
	SelectionList();
	SelectionList(const glm::vec2& pos, const glm::vec2& size, float buttonHeight, float opacity = 255.0f, float fontSize = -1.0f);

	~SelectionList() = default;

	// Sets the opacity of the selection list.
	void SetOpacity(float opacity);

	// Adds new category to the selection list.
	void AddCategory(const std::string_view& name);

	// Adds new selection element to the selection list.
	void AddElement(const std::vector<std::string>& categoryValues, int value);

	// Clears all selection elements from the selection list.
	void Clear();

	// Resets the current selected element pointer to null.
	void Reset();

	// Updates the selection list.
	void Update(const float& deltaTime);

	// Render the selection list.
	void Render(float masterOpacity) const;

	// Returns the value contained by the current selected list element.
	// If no element in the list is selected, -1 is returned.
	int GetCurrentSelected() const;

	// Returns the vector containing the list elements.
	std::vector<Element>& GetListElements();

	// Returns the position of the selection list.
	const glm::vec2& GetPosition() const;

	// Returns the size of the selection list.
	const glm::vec2& GetSize() const;

	// Returns the opacity of the selection list.
	const float& GetOpacity() const;

};

#endif
