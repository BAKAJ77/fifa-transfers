#ifndef MY_CLUB_SECTION_H
#define MY_CLUB_SECTION_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/user_profile.h>

class MyClub : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;

	UserProfile* currentUser;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static MyClub* GetAppState();
};

#endif