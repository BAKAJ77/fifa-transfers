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

	bool lengthInvalid, wageInvalid, releaseClauseInvalid;
	bool exitState, onNegotiationCooldown, renewingContract;
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
	void SetNegotiatingPlayer(Player* player, bool renewingContract);

	// Returns TRUE if the player avoided negotiating with the user.
	// This could be because of negotiation cooldowns, the player being too good for the user's club etc.
	bool wasNegotiationsAvoided() const;
};

#endif