#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class MainMenu : public AppState
{
private:
	UserInterface userInterface;
	float bkgOpacity, logoOpacity;
	bool paused;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	// Sets the opacity of the title logo.
	void SetLogoOpacity(float opacity);

	// Returns the opacity of the title logo.
	float GetLogoOpacity() const;

	static MainMenu* GetAppState();
};

#endif
