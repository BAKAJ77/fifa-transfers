#include <serialization/league_group.h>
#include <util/logging_system.h>

#include <cassert>

League::League() :
    id(0), tier(0), autoPromotion(-1), playoffs(-1), relegation(-1), titleBonus(0.0f)
{}

League::League(const std::string_view& name, const std::string_view& nation, uint16_t id, uint16_t tier, int autoPromotion, int playoffs, int relegation,
    float titleBonus, const std::vector<CompetitionLink>& linkedComps, const std::vector<Club*> clubs) :
    name(name), nation(nation), id(id), tier(tier), autoPromotion(autoPromotion), playoffs(playoffs), relegation(relegation), titleBonus(titleBonus), 
    clubs(clubs), linkedCompetitions(linkedComps)
{}

void League::SetName(const std::string_view& name)
{
    this->name = name;
}

void League::SetNation(const std::string_view& nation)
{
    this->nation = nation;
}

void League::SetTier(uint16_t tier)
{
    this->tier = tier;
}

void League::SetAutoPromotionThreshold(int threshold)
{
    this->autoPromotion = threshold;
}

void League::SetPlayoffsThreshold(int threshold)
{
    this->playoffs = threshold;
}

void League::SetRelegationThreshold(int threshold)
{
    this->relegation = threshold;
}

void League::SetTitleBonus(float multiplier)
{
    this->titleBonus = multiplier;
}

void League::AddClub(Club* club)
{
    // Make sure a valid pointer to a club was given
    assert(club != nullptr);

    // Make sure the club isn't already in the league
    for (const Club* leagueClub : this->clubs)
    {
        if (leagueClub->GetID() == club->GetID())
        {
            LogSystem::GetInstance().OutputLog("The club is already present in the league (Club ID: " + std::to_string(club->GetID()) + ")",
                Severity::WARNING);
            return;
        }
    }

    // Add the club to the league
    club->SetLeague(this->id);
    this->clubs.emplace_back(club);
}

void League::RemoveClub(Club* club)
{
    // Make sure a valid pointer to a club was given
    assert(club != nullptr);

    // Attempt to remove the club from the league
    for (auto iterator = this->clubs.begin(); iterator != this->clubs.end(); iterator++)
    {
        if ((*iterator)->GetID() == club->GetID())
        {
            this->clubs.erase(iterator);
            return;
        }
    }

    // The club couldn't be found in the league
    LogSystem::GetInstance().OutputLog("The club couldn't be found in the league (Club ID: " + std::to_string(club->GetID()) + ")",
        Severity::WARNING);
}

std::vector<Club*>& League::GetClubs()
{
    return this->clubs;
}

const std::vector<Club*>& League::GetClubs() const
{
    return this->clubs;
}

std::string_view League::GetName() const
{
    return this->name;
}

std::string_view League::GetNation() const
{
    return this->nation;
}

const uint16_t& League::GetID() const
{
    return this->id;
}

const uint16_t& League::GetTier() const
{
    return this->tier;
}

const int& League::GetAutoPromotionThreshold() const
{
    return this->autoPromotion;
}

const int& League::GetPlayoffsThreshold() const
{
    return this->playoffs;
}

const int& League::GetRelegationThreshold() const
{
    return this->relegation;
}

const std::vector<League::CompetitionLink>& League::GetLinkedCompetitions() const
{
    return this->linkedCompetitions;
}
