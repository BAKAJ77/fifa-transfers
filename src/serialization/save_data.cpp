#include <serialization/save_data.h>
#include <serialization/json_loader.h>
#include <util/directory_system.h>
#include <util/logging_system.h>

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

void SaveData::LoadCupsFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t cupID = 1;
    while (dataRoot.contains(std::to_string(cupID)))
    {
        // Add the cup competition to the database
        const std::string cupIDStr = std::to_string(cupID);

        KnockoutCup cup = { cupID, dataRoot[cupIDStr]["name"].get<std::string>(), dataRoot[cupIDStr]["nations"].get<std::vector<std::string>>(),
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
        if (dataRoot[std::to_string(leagueID)]["supported"].get<bool>())
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
        }

        ++leagueID;
    }
}

void SaveData::LoadClubsFromJSON(const nlohmann::json& dataRoot, bool loadingDefault)
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

        if (loadingDefault)
        {
            this->clubDatabase.push_back({ dataRoot[clubIDStr]["name"].get<std::string>(), clubID, dataRoot[clubIDStr]["leagueID"].get<uint16_t>(), 0, 
                0, players });
        }
        else
        {
            this->clubDatabase.push_back({ dataRoot["clubs"][clubIDStr]["name"].get<std::string>(), clubID,
                dataRoot["clubs"][clubIDStr]["leagueID"].get<uint16_t>(), dataRoot["clubs"][clubIDStr]["transferBudget"].get<int>(),
                dataRoot["clubs"][clubIDStr]["wageBudget"].get<int>(), players });
        }

        ++clubID;
    }
}

void SaveData::LoadPlayersFromJSON(const nlohmann::json& dataRoot, bool loadingDefault)
{
    uint16_t playerID = 1;
    while (dataRoot.contains(std::to_string(playerID)))
    {
        // Add the player to the database
        const std::string playerIDStr = std::to_string(playerID);
        
        if (loadingDefault)
        {
            this->playerDatabase.push_back({ dataRoot[playerIDStr]["name"].get<std::string>(), dataRoot[playerIDStr]["nation"].get<std::string>(),
                dataRoot[playerIDStr]["preferredFoot"].get<std::string>(), playerID, dataRoot[playerIDStr]["currentClubID"].get<uint16_t>(),
                dataRoot[playerIDStr]["parentClubID"].get<uint16_t>(), dataRoot[playerIDStr]["positionID"].get<uint16_t>(),
                dataRoot[playerIDStr]["age"].get<int>(), dataRoot[playerIDStr]["overall"].get<int>(), dataRoot[playerIDStr]["potential"].get<int>(),
                dataRoot[playerIDStr]["value"].get<int>(), dataRoot[playerIDStr]["wage"].get<int>(), dataRoot[playerIDStr]["releaseClause"].get<int>(),
                dataRoot[playerIDStr]["expiryYear"].get<int>(), false, false, false });
        }
        else
        {
            this->playerDatabase.push_back({ dataRoot["players"][playerIDStr]["name"].get<std::string>(), 
                dataRoot["players"][playerIDStr]["nation"].get<std::string>(), dataRoot["players"][playerIDStr]["preferredFoot"].get<std::string>(), 
                playerID, dataRoot["players"][playerIDStr]["currentClubID"].get<uint16_t>(), 
                dataRoot["players"][playerIDStr]["parentClubID"].get<uint16_t>(), dataRoot["players"][playerIDStr]["positionID"].get<uint16_t>(),
                dataRoot["players"][playerIDStr]["age"].get<int>(), dataRoot["players"][playerIDStr]["overall"].get<int>(), 
                dataRoot["players"][playerIDStr]["potential"].get<int>(), dataRoot["players"][playerIDStr]["value"].get<int>(), 
                dataRoot["players"][playerIDStr]["wage"].get<int>(), dataRoot["players"][playerIDStr]["releaseClause"].get<int>(),
                dataRoot["players"][playerIDStr]["expiryYear"].get<int>(), dataRoot["players"][playerIDStr]["loanListed"].get<bool>(),
                dataRoot["players"][playerIDStr]["transferListed"].get<bool>(), dataRoot["players"][playerIDStr]["transfersBlocked"].get<bool>() });
        }

        ++playerID;
    }
}

void SaveData::Write(float& currentProgress, std::mutex& mutex)
{
    // Open the save file (it will be generated if it's a new save file)
    JSONLoader file(Util::GetAppDataDirectory() + "data/saves/" + this->name + ".json");
    
    // Calculate the progress increase per action
    const int numActions = (int)(this->clubDatabase.size() + this->playerDatabase.size() + this->users.size());
    const float progressPerAction = 95.0f / (float)numActions;

    // Write the data of all clubs into the JSON structure
    for (const Club& club : this->clubDatabase)
    {
        this->ConvertClubToJSON(file.GetRoot(), club);

        // Update the current progress tracker
        {
            std::scoped_lock lock(mutex);
            currentProgress += progressPerAction;
        }
    }

    // Write the data of all players into the JSON structure
    for (const Player& player : this->playerDatabase)
    {
        this->ConvertPlayerToJSON(file.GetRoot(), player);

        // Update the current progress tracker
        {
            std::scoped_lock lock(mutex);
            currentProgress += progressPerAction;
        }
    }

    // Write the data of all users into the JSON structure
    for (const UserProfile& user : this->users)
    {
        this->ConvertUserProfileToJSON(file.GetRoot(), user);

        // Update the current progress tracker
        {
            std::scoped_lock lock(mutex);
            currentProgress += progressPerAction;
        }
    }

    file.Close();
    file.Clear();

    // Open the saves metadata file
    file.Open(Util::GetAppDataDirectory() + "data/saves.json");

    // Get the next free save ID
    uint16_t nextID = 1;
    while (file.GetRoot().contains(std::to_string(nextID)))
        nextID++;

    // Write the new save filename to the saves metadata file
    file.GetRoot()[std::to_string(nextID)]["filename"] = this->name + ".json";

    // Update the current progress tracker
    {
        std::scoped_lock lock(mutex);
        currentProgress = 100.0f;
    }
}

void SaveData::ConvertClubToJSON(nlohmann::json& root, const Club& club) const
{
    root["clubs"][std::to_string(club.GetID())]["name"] = club.GetName();
    root["clubs"][std::to_string(club.GetID())]["leagueID"] = club.GetLeague();
    root["clubs"][std::to_string(club.GetID())]["transferBudget"] = club.GetTransferBudget();
    root["clubs"][std::to_string(club.GetID())]["wageBudget"] = club.GetWageBudget();
}

void SaveData::ConvertPlayerToJSON(nlohmann::json& root, const Player& player) const
{
    root["players"][std::to_string(player.GetID())]["name"] = player.GetName();
    root["players"][std::to_string(player.GetID())]["nation"] = player.GetNation();
    root["players"][std::to_string(player.GetID())]["preferredFoot"] = player.GetPreferredFoot();
    root["players"][std::to_string(player.GetID())]["currentClubID"] = player.GetCurrentClub();
    root["players"][std::to_string(player.GetID())]["parentClubID"] = player.GetParentClub();
    root["players"][std::to_string(player.GetID())]["positionID"] = player.GetPosition();

    root["players"][std::to_string(player.GetID())]["age"] = player.GetAge();
    root["players"][std::to_string(player.GetID())]["overall"] = player.GetOverall();
    root["players"][std::to_string(player.GetID())]["potential"] = player.GetPotential();
    root["players"][std::to_string(player.GetID())]["value"] = player.GetValue();
    root["players"][std::to_string(player.GetID())]["wage"] = player.GetWage();
    root["players"][std::to_string(player.GetID())]["releaseClause"] = player.GetReleaseClause();
    root["players"][std::to_string(player.GetID())]["expiryYear"] = player.GetExpiryYear();

    root["players"][std::to_string(player.GetID())]["loanListed"] = player.GetLoanListed();
    root["players"][std::to_string(player.GetID())]["transferListed"] = player.GetTransferListed();
    root["players"][std::to_string(player.GetID())]["transfersBlocked"] = player.GetTransfersBlocked();
}

void SaveData::ConvertUserProfileToJSON(nlohmann::json& root, const UserProfile& user) const
{
    root["users"][std::to_string(user.GetID())]["name"] = user.GetName();
    root["users"][std::to_string(user.GetID())]["clubID"] = user.GetClub()->GetID();

    for (const UserProfile::CompetitionData& compData : user.GetCompetitionData())
    {
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["competitionID"] = compData.compID;

        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["currentScored"] = compData.currentScored;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["currentConceded"] = compData.currentConceded;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["currentWins"] = compData.currentWins;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["currentDraws"] = compData.currentDraws;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["currentLosses"] = compData.currentLosses;

        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["totalScored"] = compData.totalScored;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["totalConceded"] = compData.totalConceded;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["totalWins"] = compData.totalWins;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["totalDraws"] = compData.totalDraws;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["totalLosses"] = compData.totalLosses;

        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["mostScored"] = compData.mostScored;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["mostConceded"] = compData.mostConceded;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["mostWins"] = compData.mostWins;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["mostDraws"] = compData.mostDraws;
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["mostLosses"] = compData.mostLosses;

        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["titlesWon"] = compData.titlesWon;
    }
}

UserProfile* SaveData::GetUser(uint16_t id)
{
    for (UserProfile& user : this->users)
    {
        if (user.GetID() == id)
            return &user;
    }

    LogSystem::GetInstance().OutputLog("No user profile was found matching the ID: " + std::to_string(id), Severity::WARNING);
    return nullptr;
}

Player* SaveData::GetPlayer(uint16_t id)
{
    for (Player& player : this->playerDatabase)
    {
        if (player.GetID() == id)
            return &player;
    }

    LogSystem::GetInstance().OutputLog("No player was found matching the ID: " + std::to_string(id), Severity::WARNING);
    return nullptr;
}

Club* SaveData::GetClub(uint16_t id)
{
    for (Club& club : this->clubDatabase)
    {
        if (club.GetID() == id)
            return &club;
    }

    LogSystem::GetInstance().OutputLog("No club was found matching the ID: " + std::to_string(id), Severity::WARNING);
    return nullptr;
}

League* SaveData::GetLeague(uint16_t id)
{
    for (League& league : this->leagueDatabase)
    {
        if (league.GetID() == id)
            return &league;
    }

    LogSystem::GetInstance().OutputLog("No league was found matching the ID: " + std::to_string(id), Severity::WARNING);
    return nullptr;
}

KnockoutCup* SaveData::GetCup(uint16_t id)
{
    for (KnockoutCup& cup : this->cupDatabase)
    {
        if (cup.GetID() == id)
            return &cup;
    }

    LogSystem::GetInstance().OutputLog("No cup was found matching the ID: " + std::to_string(id), Severity::WARNING);
    return nullptr;
}

std::vector<UserProfile>& SaveData::GetUsers()
{
    return this->users;
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
