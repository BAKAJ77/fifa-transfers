#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class MainMenu : public AppState
{
private:
	UserInterface menuGUI;
	float bkgOpacity;
	bool paused;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static MainMenu* GetAppState();
};

#endif
