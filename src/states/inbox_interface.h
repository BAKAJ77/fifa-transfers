#ifndef INBOX_INTERFACE_H
#define INBOX_INTERFACE_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/club_entity.h>

class InboxInterface : public AppState
{
public:
	enum class InboxType
	{
		GENERAL,
		TRANSFERS
	};

	struct AgreedTransfer
	{
		uint16_t sellingClubID;
		const Club::Transfer* transferMsg;
		bool finishedNegotiating;
	};
private:
	UserInterface userInterface;
	FontPtr font;
	InboxType type;

	AgreedTransfer selectedAgreedTransfer;
	mutable bool insufficientTransferFunds;
	bool exitState;
private:
	void RenderInsufficientTransferFundsText() const;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
	bool OnPauseTransitionUpdate(const float deltaTime) override;
	bool OnResumeTransitionUpdate(const float deltaTime) override;
public:
	static InboxInterface* GetAppState();

	// Sets whether the inbox is a GENERAL inbox or a TRANSFERS inbox.
	void SetInboxType(InboxType type);

	// Loads the user's general messages inbox into the selection list.
	void LoadGeneralMessages();

	// Loads the user's transfer messages inbox into the selection list.
	void LoadTransferMessages();
};

#endif