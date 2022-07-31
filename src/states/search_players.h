#ifndef SEARCH_PLAYERS_H
#define SEARCH_PLAYERS_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class SearchPlayers : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
	std::string previousNameEntry, previousClubEntry, previousPositionEntry;
	bool exitState;
private:
	// Fills the selection list with players who match the filters specified by the user.
	void UpdateSelectionList();
protected:
	void Init() override;
	void Destroy() override;

	void Resume() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static SearchPlayers* GetAppState();

	// Clears the filter inputs and the player selection list.
	void Reset();
};

#endif
