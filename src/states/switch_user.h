#ifndef SWITCH_USER_H
#define SWITCH_USER_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class SwitchUser : public AppState
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
public:
	static SwitchUser* GetAppState();
};

#endif
