#ifndef SAVE_WRITING_H
#define SAVE_WRITING_H

#include <core/application_state.h>
#include <interface/user_interface.h>

#include <mutex>

class SaveWriting : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;

	mutable std::mutex mutex;
	float savingProgress, opacity;
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
};
#endif