#ifndef INBOX_MENU_H
#define INBOX_MENU_H

#include <core/application_state.h>
#include <graphics/texture_loader.h>
#include <interface/user_interface.h>

class InboxMenu : public AppState
{
private:
	UserInterface userInterface;
	TextureBuffer2DPtr circleTex;
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
	static InboxMenu* GetAppState();
};

#endif
