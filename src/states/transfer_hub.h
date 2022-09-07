#ifndef TRANSFER_HUB_H
#define TRANSFER_HUB_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class TransferHub : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
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
	static TransferHub* GetAppState();
};

#endif
