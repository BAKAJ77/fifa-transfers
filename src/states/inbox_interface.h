#ifndef INBOX_INTERFACE_H
#define INBOX_INTERFACE_H

#include <core/application_state.h>
#include <interface/user_interface.h>

class InboxInterface : public AppState
{
public:
	enum class InboxType
	{
		GENERAL,
		TRANSFERS
	};
private:
	UserInterface userInterface;
	FontPtr font;
	InboxType type;
	bool exitState;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static InboxInterface* GetAppState();

	// Sets whether the inbox is a GENERAL inbox or a TRANSFERS inbox.
	void SetInboxType(InboxType type);
};

#endif