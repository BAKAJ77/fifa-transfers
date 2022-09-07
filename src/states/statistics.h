#ifndef STATISTICS_H
#define STATISTICS_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/user_profile.h>

class Statistics : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
	UserProfile* displayedUser;

	int competitionIndex;
	bool exitState;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static Statistics* GetAppState();
};

#endif
