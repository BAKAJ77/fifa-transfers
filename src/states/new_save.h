#ifndef NEW_GAME_H
#define NEW_GAME_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class NewSave : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;

	int randomisePotentials;
	bool goBackToPlayMenu, saveNameInvalid, playerCountInvalid, growthSystemInvalid, randomisePotentialInvalid;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static NewSave* GetAppState();

	// Returns TRUE if player potentials have been set to be randomised in the new save.
	bool ShouldRandomisePotentials() const;
};

#endif
