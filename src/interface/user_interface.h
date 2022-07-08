#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <core/window_frame.h>
#include <interface/button_base.h>
#include <interface/text_input_field.h>
#include <interface/radio_buttons.h>
#include <interface/drop_down.h>

#include <string>
#include <vector>
#include <unordered_map>

class UserInterface
{
private:
	WindowFramePtr appWindow;
	std::vector<ButtonBase*> standaloneButtons;
	std::unordered_map<std::string, TextInputField> standaloneTextFields;

	std::unordered_map<std::string, RadioButtonGroup> radioButtonGroups;
	std::unordered_map<std::string, DropDown> dropDowns;

	float animationSpeed, opacity;
public:
	UserInterface();
	UserInterface(WindowFramePtr window, float animationSpeed, float opacity);

	~UserInterface();

	// Sets the animation speed of all elements (e.g. buttons) in the interface.
	void SetAnimationSpeed(float speed);

	// Sets the master opacity of the user interface.
	void SetOpacity(float opacity);

	// Adds a standalone button to the interface.
	// NOTE: The button object given must be heap allocated i.e with the 'new' keyword.
	void AddStandaloneButton(ButtonBase* button);

	// Adds a standalone text field to the interface.
	void AddStandaloneTextField(const std::string_view& id, const TextInputField& field);

	// Adds a radio button group to the interface.
	void AddRadioButtonGroup(const std::string_view& id, const RadioButtonGroup& group);

	// Adds a drop down to the interface.
	void AddDropDown(const std::string_view& id, DropDown&& dropDown);

	// Updates the interface.
	void Update(const float& deltaTime);

	// Renders the interface.
	void Render() const;

	// Returns the standalone text field matching the ID given.
	TextInputField* GetStandaloneTextField(const std::string_view& id);

	// Returns the vector containing the standalone buttons in the interface.
	const std::vector<ButtonBase*>& GetStandaloneButtons(); 

	// Returns the radio button group matching the ID given.
	RadioButtonGroup* GetRadioButtonGroup(const std::string_view& id);

	// Returns the drop down matching the ID given.
	DropDown* GetDropDown(const std::string_view& id);

	// Returns the master opacity of the user interface. 
	const float& GetOpacity() const;
};

#endif