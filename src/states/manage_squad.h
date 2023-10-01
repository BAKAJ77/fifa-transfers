#ifndef MANAGE_SQUAD_H
#define MANAGE_SQUAD_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/club_entity.h>

class ManageSquad : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
	Club* currentUserClub;
	bool exitState;
protected:
	void Init() override;
	void Destroy() override;

	void Resume() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static ManageSquad* GetAppState();

	// Reloads the players in the squad into the selection list.
	void ReloadSquad();
};

#endif
