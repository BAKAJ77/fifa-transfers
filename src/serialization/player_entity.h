#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <string>

class Player
{
private:
	std::string name, nation, preferredFoot;
	uint16_t id, clubID, positionID;
	int age, overall, potential, value, wage, releaseClause, expiryYear;
	bool transferListed, transfersBlocked;
public:
	Player();
	Player(const std::string_view& name, const std::string_view& nation, const std::string_view& preferredFoot, uint16_t id, uint16_t clubID,
		uint16_t positionID, int age, int overall, int potential, int value, int wage, int releaseClause, int expiryYear, bool transferListed, 
		bool transfersBlocked);

	~Player() = default;

	// Sets the name of the player.
	void SetName(const std::string_view& name);

	// Sets the nation of the player.
	void SetNation(const std::string_view& nation);

	// Sets the preferred foot of the player.
	void SetPreferredFoot(const std::string_view& foot);

	// Sets the club which the player belongs to.
	void SetClub(uint16_t id);

	// Sets the position of the player.
	void SetPosition(uint16_t id);

	// Sets the age of the player.
	void SetAge(int age);

	// Sets the overall of the player.
	void SetOverall(int overall);

	// Sets the potential of the player.
	void SetPotential(int potential);

	// Sets the value of the player.
	void SetValue(int value);

	// Sets the wage of the player.
	void SetWage(int wage);

	// Sets the release clause of the player.
	void SetReleaseClause(int amount);

	// Sets the expiry year of the player.
	void SetExpiryYear(int year);

	// Sets the transfer listed status of the player.
	void SetTransferListed(bool listed);

	// Sets the incoming transfers blocked status of the player.
	void SetTransfersBlocked(bool block);

	// Returns the name of the player.
	std::string_view GetName() const;

	// Returns the nation of the player.
	std::string_view GetNation() const;

	// Returns the preferred foot of the player.
	std::string_view GetPreferredFoot() const;

	// Returns the ID of the player.
	uint16_t GetID() const;

	// Returns the ID of the player's club.
	uint16_t GetClub() const;

	// Returns the ID of the player's position.
	uint16_t GetPosition() const;

	// Returns the age of the player.
	int GetAge() const;

	// Returns the overall rating of the player.
	int GetOverall() const;

	// Returns the potential rating of the player.
	int GetPotential() const;

	// Returns the value of the player.
	int GetValue() const;

	// Returns the wage of the player.
	int GetWage() const;

	// Returns the release clause of the player.
	int GetReleaseClause() const;

	// Returns the year which the player's contract ends.
	int GetExpiryYear() const;

	// Returns TRUE if the player is transfer listed.
	bool GetTransferListed() const;

	// Returns TRUE if all transfers for the player is blocked.
	bool GetTransfersBlocked() const;
};

#endif
