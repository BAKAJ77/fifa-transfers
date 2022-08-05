#ifndef RENEW_CONTRACT_H
#define RENEW_CONTRACT_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/player_entity.h>

class ContractNegotiation : public AppState
{
private:
	mutable UserInterface userInterface;
	FontPtr font;
	Player* negotiatingPlayer;
	AppState* callerAppState;

	bool* finishedNegotiating;
	bool lengthInvalid, wageInvalid, releaseClauseInvalid;
	bool exitState, wentBack, onNegotiationCooldown, leagueTierInsufficient, sellerSquadTooSmall, buyerSquadTooLarge, renewingContract;
private:
	// Checks if the all the inputs given are valid.
	bool ValidateInputs();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static ContractNegotiation* GetAppState();

	// Sets the player which the user will negotiate a new contract with.
	void SetNegotiatingPlayer(Player* player, AppState* callerAppState, bool renewingContract, bool* finishedNegotiating = nullptr);

	// Returns TRUE if the user clicked the 'BACK' button to go back.
	bool WentBack() const;
};

#endif