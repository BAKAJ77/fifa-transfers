#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <core/window_frame.h>
#include <interface/button_base.h>
#include <string>
#include <vector>

class UserInterface
{
private:
	WindowFramePtr appWindow;
	std::vector<ButtonBase*> standaloneButtons; 
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
	// Standalone buttons are in basic terms a single individual buttons, which don't have their position and size affected by
	// factors like a scroll bar etc.
	// NOTE: The button object given must be heap allocated i.e with the 'new' keyword.
	void AddStandaloneButton(ButtonBase* button);

	// Updates the interface.
	void Update(const float& deltaTime);

	// Renders the interface.
	void Render() const;

	// Returns the vector containing the standalone buttons in the interface.
	const std::vector<ButtonBase*>& GetStandaloneButtons();

	// Returns the master opacity of the user interface. 
	const float& GetOpacity() const;
};

#endif