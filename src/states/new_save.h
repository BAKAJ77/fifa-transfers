#ifndef NEW_GAME_H
#define NEW_GAME_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class NewSave : public AppState
{
private:
	mutable UserInterface userInterface;
	FontPtr font;
	float logoOpacity;

	int randomisePotentials;
	bool goBackToPlayMenu, loadedDefaultDatabase, saveNameInvalid, playerCountInvalid, growthSystemInvalid, randomisePotentialInvalid, selectedLeagueInvalid;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static NewSave* GetAppState();

	// Returns TRUE if player potentials have been set to be randomised in the new save.
	bool ShouldRandomisePotentials() const;
};

#endif
