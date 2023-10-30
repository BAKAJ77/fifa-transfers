#ifndef CONTRACT_RESPONSE_H
#define CONTRACT_RESPONSE_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/player_entity.h>

class ContractResponse : public AppState
{
private:
	UserInterface userInterface;
	FontPtr font;
	Player* negotiatingPlayer;
	AppState* callerAppState;

	bool* finishedNegotiating;
	int contractLength, contractWage, contractReleaseClause;
	bool exitState, renewingContract, contractCountered, contractRejected, negotiationSuccessful;
private:
	// Generates a response from the AI on whether they accept the contract length offered by the user.
	// Returns -1 if accepted, else the number of years the AI player asks for instead is returned.
	int GenerateContractLengthResponse() const;

	// Generates a response from the AI on whether they accept the wages offered by the user.
	// Returns -1 if accepted, else the wages the AI player asks for instead is returned.
	int GenerateWageResponse() const;

	// Generates a response from the AI on whether they accept the release clause offered by the user.
	// Returns -1 if accepted, else the release clause the AI player asks for instead is returned.
	int GenerateReleaseClauseResponse(int contractLength) const;

	// Renders text telling the user whether the player accepted the contract or not.
	void RenderOfferResponse() const;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static ContractResponse* GetAppState();

	// Sets the contract which will be offered to the player given.
	void SetContractOffer(Player* player, int length, int wage, int releaseClause, bool renewingContract, AppState* callerAppState, bool* finishedNegotiating);

	// Returns TRUE if the contract negotiations was successful i.e an agreement between the user and the player.
	bool WasNegotiationsSuccessful() const;
};

#endif
