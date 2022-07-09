#ifndef USER_SETUP_H
#define USER_SETUP_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/user_profile.h>

class UserSetup : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;

	int lastSelectedLeague;
	bool goBack, managerNameInvalid, noLeagueSelected, noClubSelected;
private:
	// Checks if the inputs for the manager name, league and club are valid.
	// Returns TRUE if all the inputs are valid.
	bool CheckInputsValid();

	// Updates the club drop down list when a new league is selected.
	void UpdateClubDropDownList();

	// Sets up a new user profile object.
	UserProfile SetupUserProfile(uint16_t id, const std::string_view& name, Club& club);
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
public:
	static UserSetup* GetAppState();
};

#endif