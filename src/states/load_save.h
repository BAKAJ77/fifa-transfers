#ifndef LOAD_SAVE_H
#define LOAD_SAVE_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class LoadSave : public AppState
{
public:
	struct ExistingSave
	{
		std::string fileName;
		int playerCount, growthSystemID;
	};
private:
	UserInterface userInterface;
	FontPtr font;

	std::vector<ExistingSave> existingSaves;
	float logoOpacity;
	bool goBack;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static LoadSave* GetAppState();

	// Returns the existing save that was selected to be loaded.
	const ExistingSave& GetSelectedExistingSave();
};

#endif
