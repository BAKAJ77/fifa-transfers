#ifndef SAVE_WRITING_H
#define SAVE_WRITING_H

#include <core/application_state.h>
#include <interface/user_interface.h>

#include <mutex>

class SaveWriting : public AppState
{
private:
	FontPtr font;
	mutable std::mutex mutex;
	float savingProgress, opacity;

	AppState* nextAppState;
private:
	// Starts the process of writing the save data to file.
	void ExecuteSavingProcess();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static SaveWriting* GetAppState();

	// Sets the next state that is executed after the save writting process is complete.
	// If nullptr is passed, the save writting app state will only be popped from the app state stack.
	void SetNextState(AppState* appState);
};
#endif