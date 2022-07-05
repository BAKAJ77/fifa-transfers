#ifndef USER_SETUP_H
#define USER_SETUP_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class UserSetup : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static UserSetup* GetAppState();
};

#endif