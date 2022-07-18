#ifndef MAIN_GAME_H
#define MAIN_GAME_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class MainGame : public AppState
{
private:
	UserInterface userInterface;
	float bkgOpacity;

	AppState* currentParallelState;
	bool changeParallelState, savingProgress;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static MainGame* GetAppState();

	// Sets whether the app state should continue updating while paused.
	void SetUpdateWhilePaused(bool enable);

	// Returns TRUE if the parallel state has been requested to change.
	bool ShouldChangeParallelState() const;
};

#endif