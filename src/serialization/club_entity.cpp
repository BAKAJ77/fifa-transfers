#include <serialization/club_entity.h>
#include <serialization/save_data.h>
#include <util/logging_system.h>
#include <util/random_engine.h>

#include <cassert>
#include <cmath>

Club::Club() :
    id(0), leagueID(0), transferBudget(0), wageBudget(0), initialTransferBudget(0), initialWageBudget(0)
{}

Club::Club(const std::string_view& name, uint16_t id, uint16_t leagueID, int transferBudget, int wageBudget, int initialTransferBudget, int initialWageBudget, 
    const std::vector<TrainingStaff>& trainingStaffGroups, const std::vector<Player*>& players, const std::vector<Objective>& objectives, 
    const std::vector<GeneralMessage>& generalMessages, const std::vector<Transfer>& transferMessages) :
    name(name), id(id), leagueID(leagueID), transferBudget(transferBudget), wageBudget(wageBudget), initialTransferBudget(initialTransferBudget),
    initialWageBudget(initialWageBudget), trainingStaffGroups(trainingStaffGroups), players(players), objectives(objectives), generalMessages(generalMessages), 
    transferMessages(transferMessages)
{
    // Sort the club players (based on their overall rating) in descending order
    std::sort(this->players.begin(), this->players.end(), [](Player* first, Player* second) { return first->GetOverall() > second->GetOverall(); });
}

void Club::SetName(const std::string_view& name)
{
    this->name = name;
}

void Club::SetLeague(uint16_t id)
{
    this->leagueID = id;
}

void Club::SetTransferBudget(int budget)
{
    this->transferBudget = budget;
}

void Club::SetWageBudget(int budget)
{
    this->wageBudget = budget;
}

void Club::SetInitialTransferBudget(int budget)
{
    this->initialTransferBudget = budget;
}

void Club::SetInitialWageBudget(int budget)
{
    this->initialWageBudget = budget;
}

void Club::GenerateObjectives()
{
    this->objectives.clear();
    const League* currentLeague = SaveData::GetInstance().GetCurrentLeague();

    // Generate a fair league position objective
    const std::vector<Club*>& leagueClubs = currentLeague->GetClubs();
    const int clubOverall = this->GetAverageOverall();

    int numBetterClubs = 0, numEqualClubs = 0;
    for (const Club* club : leagueClubs)
    {
        if (club->GetAverageOverall() > clubOverall)
            ++numBetterClubs;
        else if (club->GetAverageOverall() == clubOverall && club->GetID() != this->id)
            ++numEqualClubs;
    }

    const int targetLeaguePosition = (numBetterClubs + 1) + RandomEngine::GetInstance().GenerateRandom<int>(0, numEqualClubs);

    if (currentLeague->GetRelegationThreshold() != -1)
    {
        this->objectives.push_back({ currentLeague->GetID(), 
            (uint16_t)std::min(targetLeaguePosition, currentLeague->GetRelegationThreshold() - 1) });
    }
    else
        this->objectives.push_back({ currentLeague->GetID(), 
            (uint16_t)std::min(targetLeaguePosition, (int)currentLeague->GetClubs().size() - 3) });

    // Generate fair targets for the cup competitions
    for (const League::CompetitionLink& comp : currentLeague->GetLinkedCompetitions())
    {
        // Only generate objectives for domestic cup competitions
        if (comp.competitionID >= 1000 && comp.competitionID <= 1003) 
            continue;

        // Total up all the better and equal clubs that compete in the same cup competition
        int totalClubsInComp = numBetterClubs = numEqualClubs = 0;

        for (const League& league : SaveData::GetInstance().GetLeagueDatabase())
        {
            for (const League::CompetitionLink& compLink : league.GetLinkedCompetitions())
            {
                if (comp.competitionID == compLink.competitionID)
                {
                    for (const Club* club : league.GetClubs())
                    {
                        if (club->GetAverageOverall() > clubOverall)
                            ++numBetterClubs;
                        else if (club->GetAverageOverall() == clubOverall && club->GetID() != this->id)
                            ++numEqualClubs;
                    }

                    totalClubsInComp += (int)league.GetClubs().size();
                    break;
                }
            }
        }

        // If there is less than (or equal to) 6 similar high achieving teams, then the target should be to win the cup
        const int totalHigherAchievingClubs = (numBetterClubs + RandomEngine::GetInstance().GenerateRandom<int>(0, numEqualClubs));
        const int targetEndRound = totalHigherAchievingClubs <= 6 ? (int)(SaveData::GetInstance().GetCup(comp.competitionID)->GetRounds().size() + 1) :
            std::max((int)(SaveData::GetInstance().GetCup(comp.competitionID)->GetRounds().size() - std::log2(std::max(totalHigherAchievingClubs, 1))), 3);

        this->objectives.push_back({ comp.competitionID, (uint16_t)targetEndRound });
    }
}

void Club::AddPlayer(Player* player)
{
    // Make sure a valid pointer to a player was given
    assert(player != nullptr);

    // Make sure the player isn't already in the club
    for (const Player* clubPlayer : this->players)
    {
        if (clubPlayer->GetID() == player->GetID())
        {
            LogSystem::GetInstance().OutputLog("The player already belongs to the club (Player ID: " + std::to_string(player->GetID()) + ")", 
                Severity::WARNING);
            return;
        }
    }

    // Add the player to the club
    player->SetClub(this->id);
    this->players.emplace_back(player);
}

void Club::RemovePlayer(Player* player)
{
    // Make sure a valid pointer to a player was given
    assert(player != nullptr);

    // Attempt to remove the player from the club
    for (auto iterator = this->players.begin(); iterator != this->players.end(); iterator++)
    {
        if ((*iterator)->GetID() == player->GetID())
        {
            this->players.erase(iterator);
            return;
        }
    }

    // The player couldn't be found in the club
    LogSystem::GetInstance().OutputLog("The player couldn't be found in the club (Player ID: " + std::to_string(player->GetID()) + ")", 
        Severity::WARNING);
}

int Club::GetAverageOverall() const
{
    // Only the top 11 players are taken into account, we assume those players are in the starting 11 of the club
    int overallTotal = 0;
    for (size_t index = 0; index < 11; index++)
        overallTotal += this->players[index]->GetOverall();

    return overallTotal / 11;
}

Club::TrainingStaff& Club::GetTrainingStaff(Club::StaffType type)
{
    TrainingStaff* returnedStaff = nullptr;
    for (TrainingStaff& staff : this->trainingStaffGroups)
    {
        if (staff.type == type)
        {
            returnedStaff = &staff;
            break;
        }
    }

    return *returnedStaff;
}

const Club::TrainingStaff& Club::GetTrainingStaff(Club::StaffType type) const
{
    const TrainingStaff* returnedStaff = nullptr;
    for (const TrainingStaff& staff : this->trainingStaffGroups)
    {
        if (staff.type == type)
        {
            returnedStaff = &staff;
            break;
        }
    }

    return *returnedStaff;
}

std::vector<Club::TrainingStaff>& Club::GetTrainingStaff()
{
    return this->trainingStaffGroups;
}

const std::vector<Club::TrainingStaff>& Club::GetTrainingStaff() const
{
    return this->trainingStaffGroups;
}

std::vector<Player*>& Club::GetPlayers()
{
    return this->players;
}

std::vector<Club::GeneralMessage>& Club::GetGeneralMessages()
{
    return this->generalMessages;
}

const std::vector<Club::GeneralMessage>& Club::GetGeneralMessages() const
{
    return this->generalMessages;
}

std::vector<Club::Transfer>& Club::GetTransferMessages()
{
    return this->transferMessages;
}

const std::vector<Club::Transfer>& Club::GetTransferMessages() const
{
    return this->transferMessages;
}

std::string_view Club::GetName() const
{
    return this->name;
}

const uint16_t& Club::GetID() const
{
    return this->id;
}

const uint16_t& Club::GetLeague() const
{
    return this->leagueID;
}

const int& Club::GetTransferBudget() const
{
    return this->transferBudget;
}

const int& Club::GetWageBudget() const
{
    return this->wageBudget;
}

const int& Club::GetInitialTransferBudget() const
{
    return this->initialTransferBudget;
}

const int& Club::GetInitialWageBudget() const
{
    return this->initialWageBudget;
}

const std::vector<Club::Objective>& Club::GetObjectives() const
{
    return this->objectives;
}

const int Club::GetTotalGoalkeepers() const
{
    int count = 0;
    for (const Player* player : this->players)
    {
        if (player->GetPosition() == 0)
            ++count;
    }

    return count;
}

const int Club::GetTotalOutfielders() const
{
    return (int)this->players.size() - this->GetTotalGoalkeepers();
}
