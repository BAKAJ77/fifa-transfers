#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include <serialization/cup_group.h>
#include <serialization/league_group.h>
#include <serialization/club_entity.h>
#include <serialization/player_entity.h>
#include <serialization/user_profile.h>

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <mutex>

class SaveData
{
public:
	enum class GrowthSystemType
	{
		OVERALL_RATING,
		SKILL_POINTS
	};

	enum class CooldownType
	{
		CONTRACT_NEGOTIATING = 0,
		TRANSFER_NEGOTIATING = 1
	};

	enum class PositionCategory
	{
		GOALKEEPER = 0,
		DEFENDER = 1,
		MIDFIELDER = 2,
		FORWARD = 3
	};

	struct Position
	{
		uint16_t id;
		std::string type;
		PositionCategory category;
	};

	struct NegotiationCooldown
	{
		uint16_t playerID;
		uint16_t clubID; // If this is set as 0, then all clubs are included in the negotiation cooldown with the player
		CooldownType type;

		int ticksRemaining;
	};

	struct PastTransfer
	{
		uint16_t playerID, fromClubID, toClubID;
		int transferFee;
	};
private:
	std::string name;
	uint8_t playerCount;
	GrowthSystemType growthSystemType;

	uint16_t currentYear;
	League* currentLeague;
	std::vector<UserProfile> users;

	std::vector<NegotiationCooldown> negotiationCooldowns;
	std::vector<PastTransfer> transferHistory;

	std::vector<KnockoutCup> cupDatabase;
	std::vector<League> leagueDatabase;
	std::vector<Club> clubDatabase;
	std::vector<Player> playerDatabase;
	std::vector<Position> positionDatabase;
private:
	// Converts the data of the club given into JSON and inserts it into the JSON object given.
	void ConvertClubToJSON(nlohmann::json& root, const Club& club) const;

	// Converts the data of the player given into JSON and inserts it into the JSON object given.
	void ConvertPlayerToJSON(nlohmann::json& root, const Player& player) const;

	// Converts the data of the user given into JSON and inserts it into the JSON object given.
	void ConvertUserProfileToJSON(nlohmann::json& root, const UserProfile& user) const;

	// Converts the data of the negotiation cooldown given into JSON and inserts it into the JSON object given.
	void ConvertNegotiationCooldownToJSON(nlohmann::json& root, const NegotiationCooldown& cooldown, int index) const;

	// Converts the data of the past transfer given into JSON and inserts it into the JSON object given.
	void ConvertPastTransferToJSON(nlohmann::json& root, const PastTransfer& transfer, int index) const;
public:
	SaveData();
	SaveData(const SaveData& other) = delete;
	SaveData(SaveData&& temp) noexcept = delete;

	~SaveData() = default;

	// Sets the name of the save.
	void SetSaveName(const std::string_view& name);

	// Sets the number of users in the save.
	void SetPlayerCount(uint8_t count);

	// Sets the type of growth system used in the save.
	void SetGrowthSystem(GrowthSystemType type);

	// Sets the save's current year.
	void SetCurrentYear(uint16_t year);

	// Sets the league the users are currently competing for in this save.
	void SetCurrentLeague(League* league);

	// Loads every user profile's data in the JSON structure into the vector.
	// You must call the functions 'LoadClubsFromJSON()' before calling this one.
	void LoadUsersFromJSON(const nlohmann::json& dataRoot);

	// Loads every cup competition's data in the JSON structure into the vector.
	void LoadCupsFromJSON(const nlohmann::json& dataRoot);

	// Loads every league's data in the JSON structure into the vector. 
	// You must call the functions 'LoadClubsFromJSON()' before calling this one.
	void LoadLeaguesFromJSON(const nlohmann::json& dataRoot);

	// Loads every club's data in the JSON structure into the vector. 
	// You must call the function 'LoadPlayersFromJSON()' before calling this one.
	void LoadClubsFromJSON(const nlohmann::json& dataRoot, bool loadingDefault = true);

	// Loads every player's data in the JSON structure into the vector.
	void LoadPlayersFromJSON(const nlohmann::json& dataRoot, bool loadingDefault = true);

	// Loads every position's data in the JSON structure into the vector.
	void LoadPositionsFromJSON(const nlohmann::json& dataRoot);

	// Loads miscellaneous data in the JSON structure into their respective vectors.
	// This includes negotiation cooldown data, transfer history data etc.
	void LoadMiscellaneousFromJSON(const nlohmann::json& dataRoot);

	// Writes the contained save data into a save file.
	void Write(float& currentProgress, std::mutex& mutex);
	
	// Returns the save's current year.
	const uint16_t& GetCurrentYear() const;

	// Returns the league the users are currently competing for in this save.
	const League* GetCurrentLeague() const;

	// Returns the user profile matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	UserProfile* GetUser(uint16_t id);

	// Returns the position matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	Position* GetPosition(uint16_t id);

	// Returns the player matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	Player* GetPlayer(uint16_t id);

	// Returns the club matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	Club* GetClub(uint16_t id);

	// Returns the league matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	League* GetLeague(uint16_t id);

	// Returns the cup matching the ID given.
	// If none is found matching the ID, then nullptr is returned.
	KnockoutCup* GetCup(uint16_t id);

	// Returns the save's user profiles.
	std::vector<UserProfile>& GetUsers();

	// Returns the save's negotiation cooldowns.
	std::vector<NegotiationCooldown>& GetNegotiationCooldowns();

	// Returns the save's past transfers.
	std::vector<PastTransfer>& GetTransferHistory();

	// Returns the save's position database.
	std::vector<Position>& GetPositionDatabase();

	// Returns the save's player database.
	std::vector<Player>& GetPlayerDatabase();

	// Returns the save's club database.
	std::vector<Club>& GetClubDatabase();

	// Returns the save's league database.
	std::vector<League>& GetLeagueDatabase();

	// Returns the save's cup competition database.
	std::vector<KnockoutCup>& GetCupDatabase();

	// Returns the name of the save.
	std::string_view GetName() const;

	// Returns the number of users in the save.
	const uint8_t& GetPlayerCount() const;

	// Returns the type of growth system used in the save.
	const GrowthSystemType& GetGrowthSystemType() const;

	// Returns singleton instance object of this class.
	static SaveData& GetInstance();
};

#endif
