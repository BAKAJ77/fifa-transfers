#ifndef CLUB_ENTITY_H
#define CLUB_ENTITY_H

#include <serialization/player_entity.h>
#include <string>
#include <vector>

class Club
{
public:
	struct Objective
	{
		uint16_t compID, targetEndPosition;
	};

	struct Transfer
	{
		uint16_t biddingClubID, playerID, expirationTicks;
		int transferFee;
		bool counterOffer = false, feeAgreed = false;
	};

	struct GeneralMessage
	{
		std::string message;
		bool wasRead = false;
	};

	enum class StaffType
	{
		GOALKEEPING = 0,
		DEFENCE = 1,
		MIDFIELD = 2,
		ATTACK = 3
	};

	struct TrainingStaff
	{
		StaffType type;
		int level = 0;
	};
private:
	std::string name;
	uint16_t id, leagueID;
	int transferBudget, wageBudget;

	std::vector<TrainingStaff> trainingStaffGroups;
	std::vector<Player*> players;
	std::vector<Objective> objectives;
	std::vector<GeneralMessage> generalMessages;
	std::vector<Transfer> transferMessages;
public:
	Club();
	Club(const std::string_view& name, uint16_t id, uint16_t leagueID, int transferBudget, int wageBudget, const std::vector<TrainingStaff>& trainingStaffGroups, 
		const std::vector<Player*>& players, const std::vector<Objective>& objectives, const std::vector<GeneralMessage>& generalMessages, 
		const std::vector<Transfer>& transferMessages);

	~Club() = default;

	// Sets the name of the club.
	void SetName(const std::string_view& name);

	// Sets the league which the club belongs to.
	void SetLeague(uint16_t id);

	// Sets the transfer budget of the club.
	void SetTransferBudget(int budget);

	// Sets the wage budget of the club.
	void SetWageBudget(int budget);

	// Generates new club objectives.
	void GenerateObjectives();

	// Adds the player given to the club.
	void AddPlayer(Player* player);

	// Removes player given from the club.
	void RemovePlayer(Player* player);

	// Returns the average overall of the players in the club.
	int GetAverageOverall() const;

	// Returns the current hired training staff at the club.
	TrainingStaff& GetTrainingStaff(StaffType type);

	// Returns the current hired training staff at the club.
	const TrainingStaff& GetTrainingStaff(StaffType type) const;

	// Returns the current hired training staff at the club.
	std::vector<TrainingStaff>& GetTrainingStaff();

	// Returns the current hired training staff at the club.
	const std::vector<TrainingStaff>& GetTrainingStaff() const;

	// Returns players in the club.
	std::vector<Player*>& GetPlayers();

	// Returns the club's general messages inbox.
	std::vector<GeneralMessage>& GetGeneralMessages();

	// Returns the club's general messages inbox.
	const std::vector<GeneralMessage>& GetGeneralMessages() const;

	// Returns the club's transfer messages inbox.
	std::vector<Transfer>& GetTransferMessages();

	// Returns the club's transfer messages inbox.
	const std::vector<Transfer>& GetTransferMessages() const;

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

	// Returns the club's current season objectives.
	const std::vector<Objective>& GetObjectives() const;
};

#endif