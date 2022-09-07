#ifndef VIEW_PLAYER_H
#define VIEW_PLAYER_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/player_entity.h>

class ViewPlayer : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
	Player* displayedPlayer;
	bool exitState, alreadyOwnedPlayer, usingOverallGrowth;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static ViewPlayer* GetAppState();

	// Sets the player which the app state will display stats about.
	void SetPlayerToView(Player* player);
};


#endif