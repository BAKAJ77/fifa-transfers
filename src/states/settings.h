#ifndef SETTINGS_H
#define SETTINGS_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/json_loader.h>

class Settings : public AppState
{
private:
	FontPtr font;
	UserInterface userInterface;
	JSONLoader configFile;

	float logoOpacity;
	bool exitState;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static Settings* GetAppState();
};

#endif