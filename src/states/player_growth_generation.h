#ifndef PLAYER_GROWTH_GENERATION_H
#define PLAYER_GROWTH_GENERATION_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <unordered_map>

class PlayerGrowthGeneration : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
	std::unordered_map<uint16_t, int> improvedPlayers; // [Player ID, growthAmount]
	int userIndex;
private:
	// Fills the selection list with players who improved.
	void SetupImprovedPlayersList();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
public:
	static PlayerGrowthGeneration* GetAppState();
};

#endif
