#ifndef RECORD_COMPETITION_H
#define RECORD_COMPETITION_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/cup_group.h>
#include <serialization/user_profile.h>

class RecordCompetition : public AppState
{
	struct CompetitionStats
	{
		int scored, conceded, wins, draws, losses;
		uint16_t seasonEndPosition;
		bool wonCup = false;
	};
private:
	mutable UserInterface userInterface;
	FontPtr font;

	KnockoutCup* selectedCup; // This will be nullptr if the league competition was selected instead of a cup competition
	std::vector<CompetitionStats> recordedCompetitionStats;
	size_t userProfileIndex;
	bool exitState, completed;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static RecordCompetition* GetAppState();
};

#endif