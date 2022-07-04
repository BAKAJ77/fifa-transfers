#ifndef NEW_GAME_H
#define NEW_GAME_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class NewSave : public AppState
{
private:
	UserInterface userInterface;
	bool goBackToPlayMenu;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static NewSave* GetAppState();
};

#endif
