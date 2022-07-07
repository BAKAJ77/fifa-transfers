#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include <serialization/cup_group.h>
#include <serialization/league_group.h>
#include <serialization/club_entity.h>
#include <serialization/player_entity.h>

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class SaveData
{
public:
	enum class GrowthSystemType
	{
		OVERALL_RATING,
		SKILL_POINTS
	};
private:
	std::string name;
	uint8_t playerCount;
	GrowthSystemType growthSystemType;

	std::vector<KnockoutCup> cupDatabase;
	std::vector<League> leagueDatabase;
	std::vector<Club> clubDatabase;
	std::vector<Player> playerDatabase;
private:
	// Loads every cup competition's data in the JSON structure into the vector.
	void LoadCupsFromJSON(const nlohmann::json& dataRoot);

	// Loads every league's data in the JSON structure into the vector. 
	void LoadLeaguesFromJSON(const nlohmann::json& dataRoot);

	// Loads every club's data in the JSON structure into the vector. 
	void LoadClubsFromJSON(const nlohmann::json& dataRoot);

	// Loads every player's data in the JSON structure into the vector.
	void LoadPlayersFromJSON(const nlohmann::json& dataRoot);
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

	// Loads the players, clubs, leagues and competitions databases from their respective JSON files.
	void LoadDefaultDatabase();

	// Clears all loaded data in the database.
	void ClearDatabase();

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
