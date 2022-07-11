#ifndef MAIN_GAME_H
#define MAIN_GAME_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class MainGame : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;

	float bkgOpacity;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static MainGame* GetAppState();
};

#endif