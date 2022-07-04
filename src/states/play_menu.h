#ifndef PLAY_MENU_H
#define PLAY_MENU_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class PlayMenu : public AppState
{
private:
	UserInterface userInterface;
	bool goBackToMainMenu;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static PlayMenu* GetAppState();
};

#endif
