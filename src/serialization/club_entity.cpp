#include <serialization/club_entity.h>
#include <util/logging_system.h>

#include <cassert>

Club::Club() :
    id(0), leagueID(0), transferBudget(0)
{}

Club::Club(const std::string_view& name, uint16_t id, uint16_t leagueID, int transferBudget, const std::vector<Player*>& players) :
    name(name), id(id), leagueID(leagueID), transferBudget(transferBudget), players(players)
{}

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

void Club::AddPlayer(Player* player, bool onLoan)
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
    if (!onLoan)
        player->SetParentClub(this->id);

    player->SetCurrentClub(this->id);
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

std::vector<Player*>& Club::GetPlayers()
{
    return this->players;
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
