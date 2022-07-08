#ifndef CLUB_ENTITY_H
#define CLUB_ENTITY_H

#include <serialization/player_entity.h>
#include <string>
#include <vector>

class Club
{
private:
	std::string name;
	uint16_t id, leagueID;
	int transferBudget, wageBudget;

	std::vector<Player*> players;
public:
	Club();
	Club(const std::string_view& name, uint16_t id, uint16_t leagueID, int transferBudget, int wageBudget, const std::vector<Player*>& players);

	~Club() = default;

	// Sets the name of the club.
	void SetName(const std::string_view& name);

	// Sets the league which the club belongs to.
	void SetLeague(uint16_t id);

	// Sets the transfer budget of the club.
	void SetTransferBudget(int budget);

	// Sets the wage budget of the club.
	void SetWageBudget(int budget);

	// Adds the player given to the club.
	void AddPlayer(Player* player, bool onLoan);

	// Removes player given from the club.
	void RemovePlayer(Player* player);

	// Returns players in the club.
	std::vector<Player*>& GetPlayers();

	// Returns the name of the club.
	std::string_view GetName() const;

	// Returns the ID of the club.
	const uint16_t& GetID() const;

	// Returns the ID of the league.
	const uint16_t& GetLeague() const;

	// Returns the club's transfer budget.
	const int& GetTransferBudget() const;

	// Returns the club's wage budget.
	const int& GetWageBudget() const;
};

#endif