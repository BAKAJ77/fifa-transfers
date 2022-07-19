#ifndef END_COMPETITION_H
#define END_COMPETITION_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class EndCompetition : public AppState
{
private:
	mutable UserInterface userInterface;
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
	static EndCompetition* GetAppState();

	// Returns the amount of competitions that has not been completed.
	int GetAmountOfIncompleteCompetitions() const;

	// Returns the competition selection list
	SelectionList& GetCompetitionSelectionList() const;
};

#endif
