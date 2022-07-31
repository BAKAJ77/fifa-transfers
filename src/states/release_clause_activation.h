#ifndef RELEASE_CLAUSE_ACTIVATION_H
#define RELEASE_CLAUSE_ACTIVATION_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/player_entity.h>

class ReleaseClauseActivation : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
	Player* targettedPlayer;

	int releaseClauseFee;
	bool exitState;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static ReleaseClauseActivation* GetAppState();

	// Sets the player who the user wants to activate the release clause for.
	void SetTargettedPlayer(Player* player);
};


#endif
