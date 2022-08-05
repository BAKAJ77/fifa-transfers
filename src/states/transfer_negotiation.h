#ifndef TRANSFER_NEGOTIATION_H
#define TRANSFER_NEGOTIATION_H

#include <core/application_state.h>
#include <serialization/player_entity.h>
#include <serialization/club_entity.h>
#include <interface/user_interface.h>

class TransferNegotiation : public AppState
{
private:
	mutable UserInterface userInterface;
	FontPtr font;
	Player* targettedPlayer;
	const Club::Transfer* existingTransferNegotiation;
	
	bool exitState, onNegotiationCooldown, alreadyNegotiating, playerNotForSale, submittedResponse;
	bool actionInvalid, bidAmountInvalid;
private:
	// Returns TRUE if all the user inputs are valid.
	bool ValidateInput();
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static TransferNegotiation* GetAppState();

	// Sets the player who the user is submitting a bid for.
	// If 'existingTransferNegotiation' is left as nullptr, then it is assumed that this is a opening transfer bid.
	void SetTargettedPlayer(Player* player, const Club::Transfer* existingTransferNegotiation = nullptr);
};

#endif
