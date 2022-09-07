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
		bool wonCup = false, wonPlayoffs = false;
	};
private:
	mutable UserInterface userInterface;
	FontPtr font;

	KnockoutCup* selectedCup; // This will be nullptr if the league competition was selected instead of a cup competition
	std::vector<CompetitionStats> recordedCompetitionStats;
	size_t userProfileIndex;
	bool exitState, completed, goalsScoredInvalid, goalsConcededInvalid, gamesWonInvalid, gamesDrawnInvalid, gamesLostInvalid, roundsInvalid,
		wonCupInvalid, tablePositionInvalid, wonPlayOffsInvalid;
private:
	// Generates transfers outbound from AI clubs for players of the clubs controlled by the users in this save.
	void GenerateAIOutboundTransfers();

	// Generates responses to pending transfer messages in the inboxes of AI clubs.
	void HandleAIClubsTransferResponses();

	// Updates the remaining ticks of every existing active negotiation cooldown.
	void UpdateNegotiationCooldowns();

	// Updates the remaining ticks of transfer messages in every club's inbox.
	void UpdateTransferMessagesTicks();

	// Updates the save's database states e.g. negotiation cooldown tick, AI transfer responses etc.
	void UpdateSaveDatabaseState();

	// Handles the generation of the AI club's new player's contract and moving the player to the AI club given.
	void HandleAITransferCompletion(Club& buyerClub, Club& sellerClub, Player& player, int transferFee, bool activatedReleaseClause = false);

	// Returns TRUE if the all the inputs given are valid.
	bool ValidateInputs();

	// Clears all the inputs entered into the UI elements e.g. text boxes, drop downs etc.
	void ClearAllInputs();
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