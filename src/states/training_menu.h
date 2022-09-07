#ifndef TRAINING_MENU_H
#define TRAINING_MENU_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/user_profile.h>

class TrainingMenu : public AppState
{
private:
	struct AvailableStaff
	{
		Club::TrainingStaff details;
		int price;
	};
private:
	UserInterface userInterface;
	FontPtr font;
	std::vector<AvailableStaff> purchasableStaff;
	bool exitState;
private:
	void LoadAvailableStaff(const UserProfile* currentUser);
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static TrainingMenu* GetAppState();
};

#endif
