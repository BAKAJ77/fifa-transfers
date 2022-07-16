#ifndef CONTINUE_GAME_H
#define CONTINUE_GAME_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class ContinueGame : public AppState
{
private:
	UserInterface userInterface;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static ContinueGame* GetAppState();
};

#endif