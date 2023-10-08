#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <core/window_frame.h>
#include <interface/button_base.h>
#include <interface/text_input_field.h>
#include <interface/radio_buttons.h>
#include <interface/drop_down.h>
#include <interface/selection_list.h>
#include <interface/tick_box.h>

#include <string>
#include <vector>
#include <unordered_map>

class UserInterface
{
private:
	WindowFramePtr appWindow;
	std::vector<ButtonBase*> buttons;
	std::vector<std::pair<std::string, TextInputField>> textFields;

	std::unordered_map<std::string, RadioButtonGroup> radioButtonGroups;
	std::unordered_map<std::string, SelectionList> selectionLists;
	std::unordered_map<std::string, DropDown> dropDowns;
	std::unordered_map<std::string, TickBox> tickBoxes;

	float animationSpeed, opacity;
public:
	UserInterface();
	UserInterface(WindowFramePtr window, float animationSpeed, float opacity);

	~UserInterface();

	// Sets the animation speed of all elements (e.g. buttons) in the interface.
	void SetAnimationSpeed(float speed);

	// Sets the master opacity of the user interface.
	void SetOpacity(float opacity);

	// Adds a button to the interface.
	// NOTE: The button object given must be heap allocated i.e with the 'new' keyword.
	void AddButton(ButtonBase* button);

	// Adds a text field to the interface.
	void AddTextField(const std::string_view& id, const TextInputField& field);

	// Adds a radio button group to the interface.
	void AddRadioButtonGroup(const std::string_view& id, const RadioButtonGroup& group);

	// Adds a drop down to the interface.
	void AddDropDown(const std::string_view& id, const DropDown& dropDown);

	// Adds a selection list to the interface.
	void AddSelectionList(const std::string_view& id, const SelectionList& list);

	// Adds a tick box to the interface.
	void AddTickBox(const std::string_view& id, const TickBox& tickBox);

	// Updates the interface.
	void Update(const float& deltaTime);

	// Renders the interface.
	void Render() const;

	// Returns the vector containing the buttons in the interface.
	const std::vector<ButtonBase*>& GetButtons();

	// Returns the text field matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	TextInputField* GetTextField(const std::string_view& id);

	// Returns the radio button group matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	RadioButtonGroup* GetRadioButtonGroup(const std::string_view& id);

	// Returns the drop down matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	DropDown* GetDropDown(const std::string_view& id);

	// Returns the selection list matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	SelectionList* GetSelectionList(const std::string_view& id);

	// Returns the tick box matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	TickBox* GetTickBox(const std::string_view& id);

	// Returns the master opacity of the user interface. 
	const float& GetOpacity() const;
};

#endif