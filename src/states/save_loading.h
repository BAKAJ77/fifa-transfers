#ifndef SAVE_LOADING_H
#define SAVE_LOADING_H

#include <core/application_state.h>
#include <interface/user_interface.h>

#include <mutex>

class SaveLoading : public AppState
{
private:
	FontPtr font;
	mutable std::mutex mutex;
	float loadingProgress, opacity;
private:
	// Starts the process of loading the save data from the file.
	void ExecuteLoadingProcess();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static SaveLoading* GetAppState();
};
#endif