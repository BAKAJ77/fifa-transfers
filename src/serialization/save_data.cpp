#include <serialization/save_data.h>
#include <serialization/json_loader.h>
#include <util/directory_system.h>

SaveData::SaveData() :
    playerCount(0), growthSystemType(GrowthSystemType::SKILL_POINTS)
{}

void SaveData::SetSaveName(const std::string_view& name)
{
    this->name = name;
}

void SaveData::SetPlayerCount(uint8_t count)
{
    this->playerCount = count;
}

void SaveData::SetGrowthSystem(GrowthSystemType type)
{
    this->growthSystemType = type;
}

void SaveData::LoadDefaultDatabase()
{
    // Open the players JSON file and load every player's data
    JSONLoader playersFile("data/players.json");
    this->LoadPlayersFromJSON(playersFile.GetRoot());

    // Open the clubs JSON file and load every clubs's data
    JSONLoader clubsFile("data/clubs.json");
    this->LoadClubsFromJSON(clubsFile.GetRoot());

    // Open the leagues JSON file and load every league's data
    JSONLoader leaguesFile("data/leagues.json");
    this->LoadLeaguesFromJSON(leaguesFile.GetRoot());

    // Open the cup competitions JSON file and load every cup's data
    JSONLoader cupsFile("data/cup_competitions.json");
    this->LoadCupsFromJSON(cupsFile.GetRoot());
}

void SaveData::LoadCupsFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t cupID = 1;
    while (dataRoot.contains(std::to_string(cupID)))
    {
        // Add the cup competition to the database
        const std::string cupIDStr = std::to_string(cupID);

        KnockoutCup cup = { cupID, dataRoot[cupIDStr]["name"].get<std::string>(), dataRoot[cupIDStr]["nation"].get<std::string>(),
            dataRoot[cupIDStr]["rounds"].get<std::vector<std::string>>(), dataRoot[cupIDStr]["winnerBonus"].get<float>() };

        this->cupDatabase.emplace_back(cup);
        ++cupID;
    }
}

void SaveData::LoadLeaguesFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t leagueID = 1;
    while (dataRoot.contains(std::to_string(leagueID)))
    {
        // Fetch the clubs which are in the league
        std::vector<Club*> clubs;
        for (auto& club : this->clubDatabase)
        {
            if (club.GetLeague() == leagueID)
                clubs.emplace_back(&club);
        }

        // Add the league to the database
        const std::string leagueIDStr = std::to_string(leagueID);

        League league = { dataRoot[leagueIDStr]["name"].get<std::string>(), dataRoot[leagueIDStr]["nation"].get<std::string>(), leagueID,
            dataRoot[leagueIDStr]["tier"].get<uint16_t>(), dataRoot[leagueIDStr]["championsLeagueThreshold"].get<int>(),
            dataRoot[leagueIDStr]["europaLeagueThreshold"].get<int>(), dataRoot[leagueIDStr]["conferenceLeagueThreshold"].get<int>(),
            dataRoot[leagueIDStr]["autoPromotionThreshold"].get<int>(), dataRoot[leagueIDStr]["playoffsThreshold"].get<int>(),
            dataRoot[leagueIDStr]["relegationThreshold"].get<int>(), dataRoot[leagueIDStr]["titleBonus"].get<float>(), clubs };

        this->leagueDatabase.emplace_back(league);
        ++leagueID;
    }
}

void SaveData::LoadClubsFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t clubID = 1;
    while (dataRoot.contains(std::to_string(clubID)))
    {
        // Fetch the players which are in the club
        std::vector<Player*> players;
        for (auto& player : this->playerDatabase)
        {
            if (player.GetCurrentClub() == clubID)
                players.emplace_back(&player);
        }

        // Add the club to the database
        const std::string clubIDStr = std::to_string(clubID);

        Club club = { dataRoot[clubIDStr]["name"].get<std::string>(), clubID, dataRoot[clubIDStr]["leagueID"].get<uint16_t>(),
            dataRoot[clubIDStr]["budget"].get<int>(), players };

        this->clubDatabase.emplace_back(club);
        ++clubID;
    }
}

void SaveData::LoadPlayersFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t playerID = 1;
    while (dataRoot.contains(std::to_string(playerID)))
    {
        // Add the player to the database
        const std::string playerIDStr = std::to_string(playerID);

        Player player = { dataRoot[playerIDStr]["name"].get<std::string>(), dataRoot[playerIDStr]["nation"].get<std::string>(),
            dataRoot[playerIDStr]["preferredFoot"].get<std::string>(), playerID, dataRoot[playerIDStr]["currentClubID"].get<uint16_t>(),
            dataRoot[playerIDStr]["parentClubID"].get<uint16_t>(), dataRoot[playerIDStr]["positionID"].get<uint16_t>(),
            dataRoot[playerIDStr]["age"].get<int>(), dataRoot[playerIDStr]["overall"].get<int>(), dataRoot[playerIDStr]["potential"].get<int>(),
            dataRoot[playerIDStr]["value"].get<int>(), dataRoot[playerIDStr]["wage"].get<int>(), dataRoot[playerIDStr]["releaseClause"].get<int>(),
            dataRoot[playerIDStr]["expiryYear"].get<int>(), dataRoot[playerIDStr]["loanListed"].get<bool>(),
            dataRoot[playerIDStr]["transferListed"].get<bool>(), dataRoot[playerIDStr]["transfersBlocked"].get<bool>() };

        this->playerDatabase.emplace_back(player);
        ++playerID;
    }
}

std::vector<Player>& SaveData::GetPlayerDatabase()
{
    return this->playerDatabase;
}

std::vector<Club>& SaveData::GetClubDatabase()
{
    return this->clubDatabase;
}

std::vector<League>& SaveData::GetLeagueDatabase()
{
    return this->leagueDatabase;
}

std::vector<KnockoutCup>& SaveData::GetCupDatabase()
{
    return this->cupDatabase;
}

std::string_view SaveData::GetName() const
{
    return this->name;
}

const uint8_t& SaveData::GetPlayerCount() const
{
    return this->playerCount;
}

const SaveData::GrowthSystemType& SaveData::GetGrowthSystemType() const
{
    return this->growthSystemType;
}

SaveData& SaveData::GetInstance()
{
    static SaveData instance;
    return instance;
}
