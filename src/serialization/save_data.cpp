#include <serialization/save_data.h>
#include <serialization/json_loader.h>
#include <util/directory_system.h>
#include <util/logging_system.h>

SaveData::SaveData() :
    playerCount(0), growthSystemType(GrowthSystemType::SKILL_POINTS), currentYear(0), currentLeague(nullptr)
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

void SaveData::SetCurrentYear(uint16_t year)
{
    this->currentYear = year;
}

void SaveData::SetCurrentLeague(League* league)
{
    this->currentLeague = league;
}

void SaveData::LoadCupsFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t id = 1001;
    while (dataRoot.contains(std::to_string(id)))
    {
        const std::string idStr = std::to_string(id);
        
        // Fetch the cup's data from the JSON element
        const std::string name = dataRoot[idStr]["name"].get<std::string>();
        const std::string region = dataRoot[idStr]["region"].get<std::string>();
        const std::vector<std::string> rounds = dataRoot[idStr]["rounds"].get<std::vector<std::string>>();

        const float winnerBonus = dataRoot[idStr]["winnerBonus"].get<float>();
        const int tier = dataRoot[idStr]["tier"].get<int>();

        // Add the cup competition to the database
        this->cupDatabase.emplace_back(KnockoutCup(id, name, region, rounds, winnerBonus, tier));

        ++id;
    }

    this->cupDatabase.shrink_to_fit();
}

void SaveData::LoadLeaguesFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t id = 1;
    while (dataRoot.contains(std::to_string(id)))
    {
        if (dataRoot[std::to_string(id)]["supported"].get<bool>())
        {
            const std::string idStr = std::to_string(id);

            // Fetch the league's data from the JSON element
            const std::string name = dataRoot[idStr]["name"].get<std::string>();
            const std::string nation = dataRoot[idStr]["nation"].get<std::string>();
            const uint16_t tier = dataRoot[idStr]["tier"].get<uint16_t>();

            const int autoPromotionThreshold = dataRoot[idStr]["autoPromotionThreshold"].get<int>();
            const int playoffsThreshold = dataRoot[idStr]["playoffsThreshold"].get<int>();
            const int relegationThreshold = dataRoot[idStr]["relegationThreshold"].get<int>();
            const float titleBonus = dataRoot[idStr]["titleBonus"].get<float>();

            // Fetch the competitions linked to this league
            std::vector<League::CompetitionLink> linkedCompetitions;
            for (const auto& comp : dataRoot[idStr]["linkedCompetitions"])
                linkedCompetitions.push_back({ comp["competitionID"].get<uint16_t>(), comp["qualifyingPositions"].get<std::vector<uint8_t>>() });

            // Fetch the clubs which are in the league
            std::vector<Club*> clubs;
            for (auto& club : this->clubDatabase)
            {
                if (club.GetLeague() == id)
                    clubs.emplace_back(&club);
            }

            // Add the league to the database
            this->leagueDatabase.emplace_back(League(name, nation, id, tier, autoPromotionThreshold, playoffsThreshold, relegationThreshold, titleBonus,
                linkedCompetitions, clubs));
        }

        ++id;
    }

    this->leagueDatabase.shrink_to_fit();
}

void SaveData::LoadUsersFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t id = 1;
    while (dataRoot["users"].contains(std::to_string(id)))
    {
        const std::string idStr = std::to_string(id);

        // Fetch the user profile's data from the JSON element
        const std::string name = dataRoot["users"][idStr]["name"].get<std::string>();
        const uint16_t clubID = dataRoot["users"][idStr]["clubID"].get<uint16_t>();
        
        std::vector<UserProfile::CompetitionData> competitionTrackingData;
        uint16_t competitionTrackID = 1;

        for (const nlohmann::json& compTrackData : dataRoot["users"][idStr]["competitionData"])
        {
            const uint16_t competitionID = compTrackData["competitionID"].get<uint16_t>();
            const uint16_t seasonEndPosition = compTrackData["seasonEndPosition"].get<uint16_t>();

            const int titlesWon = compTrackData["titlesWon"].get<int>();

            const int currentWins = compTrackData["currentWins"].get<int>();
            const int currentDraws = compTrackData["currentDraws"].get<int>();
            const int currentLosses = compTrackData["currentLosses"].get<int>();
            const int currentScored = compTrackData["currentScored"].get<int>();
            const int currentConceded = compTrackData["currentConceded"].get<int>();

            const int mostWins = compTrackData["mostWins"].get<int>();
            const int mostDraws = compTrackData["mostDraws"].get<int>();
            const int mostLosses = compTrackData["mostLosses"].get<int>();
            const int mostScored = compTrackData["mostScored"].get<int>();
            const int mostConceded = compTrackData["mostConceded"].get<int>();

            const int totalWins = compTrackData["totalWins"].get<int>();
            const int totalDraws = compTrackData["totalDraws"].get<int>();
            const int totalLosses = compTrackData["totalLosses"].get<int>();
            const int totalScored = compTrackData["totalScored"].get<int>();
            const int totalConceded = compTrackData["totalConceded"].get<int>();

            UserProfile::CompetitionData compTrackDataObj = { competitionTrackID, competitionID, seasonEndPosition, currentScored, currentConceded, 
                currentWins, currentDraws, currentLosses, totalScored, totalConceded, totalWins, totalDraws, totalLosses, mostScored, mostConceded, 
                mostWins, mostDraws, mostLosses, titlesWon };

            competitionTrackingData.emplace_back(compTrackDataObj);
            ++competitionTrackID;
        }

        // Add the user profile to the database
        this->users.emplace_back(UserProfile(id, name, *this->GetClub(clubID), competitionTrackingData));
        
        ++id;
    }

    this->users.shrink_to_fit();
}

void SaveData::LoadClubsFromJSON(const nlohmann::json& dataRoot, bool loadingDefault)
{
    const nlohmann::json* root = &dataRoot;
    if (!loadingDefault)
        root = &dataRoot["clubs"];

    uint16_t id = 1;
    while (root->contains(std::to_string(id)))
    {
        const std::string idStr = std::to_string(id);

        // Fetch the club's data from the JSON element
        const std::string name = (*root)[idStr]["name"].get<std::string>();
        const uint16_t leagueID = (*root)[idStr]["leagueID"].get<uint16_t>();

        int transferBudget = 0, wageBudget = 0;
        std::vector<Club::Objective> objectives;
        std::vector<std::string> generalMessages;
        
        if (!loadingDefault)
        {
            transferBudget = (*root)[idStr]["transferBudget"].get<int>();
            wageBudget = (*root)[idStr]["wageBudget"].get<int>();

            // Fetch the club's objectives
            if ((*root)[idStr].contains("objectives"))
            {
                for (const nlohmann::json& objective : (*root)[idStr]["objectives"])
                    objectives.push_back({ objective["compID"].get<uint16_t>(), objective["targetEndPosition"].get<uint16_t>() });
            }

            // Fetch the club's general messages inbox
            if ((*root)[idStr].contains("generalMessages"))
            {
                for (const nlohmann::json& generalMessage : (*root)[idStr]["generalMessages"])
                    generalMessages.push_back(generalMessage["message"].get<std::string>());
            }
        }

        // Fetch the players which are in the club
        std::vector<Player*> players;
        for (auto& player : this->playerDatabase)
        {
            if (player.GetClub() == id)
                players.emplace_back(&player);
        }

        // Add the club to the database
        this->clubDatabase.emplace_back(Club(name, id, leagueID, transferBudget, wageBudget, players, objectives, generalMessages));

        ++id;
    }

    this->clubDatabase.shrink_to_fit();
}

void SaveData::LoadPlayersFromJSON(const nlohmann::json& dataRoot, bool loadingDefault)
{
    const nlohmann::json* root = &dataRoot;
    if (!loadingDefault)
        root = &dataRoot["players"];

    uint16_t id = 1;
    while (root->contains(std::to_string(id)))
    {
        const std::string idStr = std::to_string(id);

        // Fetch the player's data from the JSON element
        const std::string name = (*root)[idStr]["name"].get<std::string>();
        const std::string nation = (*root)[idStr]["nation"].get<std::string>();
        const std::string preferredFoot = (*root)[idStr]["preferredFoot"].get<std::string>();

        const uint16_t clubID = (*root)[idStr]["clubID"].get<uint16_t>();
        const uint16_t positionID = (*root)[idStr]["positionID"].get<uint16_t>();

        const int age = (*root)[idStr]["age"].get<int>();
        const int overall = (*root)[idStr]["overall"].get<int>();
        const int potential = (*root)[idStr]["potential"].get<int>();
        const int value = (*root)[idStr]["value"].get<int>();
        const int wage = (*root)[idStr]["wage"].get<int>();
        const int releaseClause = (*root)[idStr]["releaseClause"].get<int>();
        const int expiryYear = (*root)[idStr]["expiryYear"].get<int>();

        bool loanListed = false, transferListed = false, transfersBlocked = false;

        if (!loadingDefault)
        {
            transferListed = (*root)[idStr]["transferListed"].get<bool>();
            transfersBlocked = (*root)[idStr]["transfersBlocked"].get<bool>();
        }

        // Add the player to the database
        this->playerDatabase.emplace_back(Player(name, nation, preferredFoot, id, clubID, positionID, age, overall, potential, value, wage, releaseClause, 
            expiryYear, transferListed, transfersBlocked));
        
        ++id;
    }

    this->playerDatabase.shrink_to_fit();
}

void SaveData::LoadPositionsFromJSON(const nlohmann::json& dataRoot)
{
    uint16_t id = 1;
    while (dataRoot.contains(std::to_string(id)))
    {
        const std::string idStr = std::to_string(id);

        // Fetch the position's data from the JSON element
        const std::string positionType = dataRoot[idStr]["position"].get<std::string>();

        // Add the position to the database
        this->positionDatabase.push_back({ id, positionType });

        ++id;
    }

    this->positionDatabase.shrink_to_fit();
}

void SaveData::LoadMiscellaneousFromJSON(const nlohmann::json& dataRoot)
{
    // Load all the negotiation cooldown data available
    if (dataRoot.contains("negotiationCooldowns"))
    {
        for (const nlohmann::json& cooldown : dataRoot["negotiationCooldowns"])
        {
            // Fetch the negotiation cooldown's data from the JSON element
            const uint16_t playerID = cooldown["playerID"].get<uint16_t>();
            const uint16_t clubID = cooldown["clubID"].get<uint16_t>();
            const CooldownType type = (CooldownType)cooldown["cooldownType"].get<int>();
            const int ticksRemaining = cooldown["ticksRemaining"].get<int>();

            // Add the negotiation cooldown to the database
            this->negotiationCooldowns.push_back({ playerID, clubID, type, ticksRemaining });
        }
    }

    // Load all the transfer history data available
    if (dataRoot.contains("transferHistory"))
    {
        for (const nlohmann::json& pastTransfer : dataRoot["transferHistory"])
        {
            // Fetch the transfer history data from the JSON element
            const uint16_t playerID = pastTransfer["playerID"].get<uint16_t>();
            const uint16_t fromClubID = pastTransfer["fromClubID"].get<uint16_t>();
            const uint16_t toClubID = pastTransfer["toClubID"].get<uint16_t>();
            const int transferFee = pastTransfer["transferFee"].get<int>();

            // Add the past transfer fetched into the database
            this->transferHistory.push_back({ playerID, fromClubID, toClubID, transferFee });
        }
    }
}

void SaveData::Write(float& currentProgress, std::mutex& mutex)
{
    // Open the save file (it will be generated if it's a new save file)
    JSONLoader file(Util::GetAppDataDirectory() + "data/saves/" + this->name + ".json");
    file.Clear();
    
    // Calculate the progress increase per action
    const int numActions = (int)(this->clubDatabase.size() + this->playerDatabase.size() + this->users.size() + this->negotiationCooldowns.size() + 
        this->transferHistory.size());

    const float progressPerAction = 95.0f / (float)numActions;

    // Write the save's current year and league
    file.GetRoot()["currentYear"] = this->currentYear;
    file.GetRoot()["currentLeagueID"] = this->currentLeague->GetID();

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

    // Write the data of all negotiation cooldowns into the JSON structure
    for (size_t index = 0; index < this->negotiationCooldowns.size(); index++)
    {
        const NegotiationCooldown& cooldown = this->negotiationCooldowns[index];
        this->ConvertNegotiationCooldownToJSON(file.GetRoot(), cooldown, (int)(index + 1));

        // Update the current progress tracker
        {
            std::scoped_lock lock(mutex);
            currentProgress += progressPerAction;
        }
    }

    // Write the data of all past transfers into the JSON structure
    for (size_t index = 0; index < this->transferHistory.size(); index++)
    {
        const PastTransfer& transfer = this->transferHistory[index];
        this->ConvertPastTransferToJSON(file.GetRoot(), transfer, (int)(index + 1));

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

    // Get the next free save ID, also search if the save written is new or not
    uint16_t nextID = 1;
    bool isNewSave = true;

    while (file.GetRoot().contains(std::to_string(nextID)))
    {
        if (file.GetRoot()[std::to_string(nextID)]["filename"].get<std::string>() == (this->name + ".json"))
        {
            isNewSave = false;
            break;
        }

        nextID++;
    }

    // Write the save metadata to the saves list file if it is a new save
    if (isNewSave)
    {
        file.GetRoot()[std::to_string(nextID)]["filename"] = this->name + ".json";
        file.GetRoot()[std::to_string(nextID)]["playerCount"] = this->playerCount;
        file.GetRoot()[std::to_string(nextID)]["growthSystem"] = (int)this->growthSystemType;
    }

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

    for (size_t index = 0; index < club.GetObjectives().size(); index++)
    {
        const Club::Objective& objective = club.GetObjectives()[index];
        root["clubs"][std::to_string(club.GetID())]["objectives"][std::to_string(index + 1)]["compID"] = objective.compID;
        root["clubs"][std::to_string(club.GetID())]["objectives"][std::to_string(index + 1)]["targetEndPosition"] = objective.targetEndPosition;
    }

    for (size_t index = 0; index < club.GetGeneralMessages().size(); index++)
        root["clubs"][std::to_string(club.GetID())]["generalMessages"][std::to_string(index + 1)]["message"] = club.GetGeneralMessages()[index];
}

void SaveData::ConvertPlayerToJSON(nlohmann::json& root, const Player& player) const
{
    root["players"][std::to_string(player.GetID())]["name"] = player.GetName();
    root["players"][std::to_string(player.GetID())]["nation"] = player.GetNation();
    root["players"][std::to_string(player.GetID())]["preferredFoot"] = player.GetPreferredFoot();
    root["players"][std::to_string(player.GetID())]["clubID"] = player.GetClub();
    root["players"][std::to_string(player.GetID())]["positionID"] = player.GetPosition();

    root["players"][std::to_string(player.GetID())]["age"] = player.GetAge();
    root["players"][std::to_string(player.GetID())]["overall"] = player.GetOverall();
    root["players"][std::to_string(player.GetID())]["potential"] = player.GetPotential();
    root["players"][std::to_string(player.GetID())]["value"] = player.GetValue();
    root["players"][std::to_string(player.GetID())]["wage"] = player.GetWage();
    root["players"][std::to_string(player.GetID())]["releaseClause"] = player.GetReleaseClause();
    root["players"][std::to_string(player.GetID())]["expiryYear"] = player.GetExpiryYear();

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
        root["users"][std::to_string(user.GetID())]["competitionData"][std::to_string(compData.id)]["seasonEndPosition"] = compData.seasonEndPosition;

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

void SaveData::ConvertNegotiationCooldownToJSON(nlohmann::json& root, const NegotiationCooldown& cooldown, int index) const
{
    root["negotiationCooldowns"][std::to_string(index)]["playerID"] = cooldown.playerID;
    root["negotiationCooldowns"][std::to_string(index)]["clubID"] = cooldown.clubID;
    root["negotiationCooldowns"][std::to_string(index)]["cooldownType"] = (int)cooldown.type;
    root["negotiationCooldowns"][std::to_string(index)]["ticksRemaining"] = cooldown.ticksRemaining;
}

void SaveData::ConvertPastTransferToJSON(nlohmann::json& root, const PastTransfer& transfer, int index) const
{
    root["transferHistory"][std::to_string(index)]["playerID"] = transfer.playerID;
    root["transferHistory"][std::to_string(index)]["fromClubID"] = transfer.fromClubID;
    root["transferHistory"][std::to_string(index)]["toClubID"] = transfer.toClubID;
    root["transferHistory"][std::to_string(index)]["transferFee"] = transfer.transferFee;
}

const uint16_t& SaveData::GetCurrentYear() const
{
    return this->currentYear;
}

const League* SaveData::GetCurrentLeague() const
{
    return this->currentLeague;
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

SaveData::Position* SaveData::GetPosition(uint16_t id)
{
    for (Position& position : this->positionDatabase)
    {
        if (position.id == id)
            return &position;
    }

    LogSystem::GetInstance().OutputLog("No position was found matching the ID: " + std::to_string(id), Severity::WARNING);
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

std::vector<SaveData::NegotiationCooldown>& SaveData::GetNegotiationCooldowns()
{
    return this->negotiationCooldowns;
}

std::vector<SaveData::PastTransfer>& SaveData::GetTransferHistory()
{
    return this->transferHistory;
}

std::vector<SaveData::Position>& SaveData::GetPositionDatabase()
{
    return this->positionDatabase;
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
