#include <serialization/user_profile.h>

UserProfile::UserProfile() :
    club(nullptr), id(0)
{}

UserProfile::UserProfile(uint16_t id, const std::string_view& name, Club& club, const std::vector<CompetitionData>& compData) :
    managerName(name), club(&club), id(id), competitionData(compData)
{}

void UserProfile::SetName(const std::string_view& name)
{
    this->managerName = name;
}

void UserProfile::SetClub(Club& club)
{
    this->club = &club;
}

std::vector<UserProfile::CompetitionData>& UserProfile::GetCompetitionData()
{
    return this->competitionData;
}

const std::vector<UserProfile::CompetitionData>& UserProfile::GetCompetitionData() const
{
    return this->competitionData;
}

Club* UserProfile::GetClub()
{
    return this->club;
}

const Club* UserProfile::GetClub() const
{
    return this->club;
}

const uint16_t& UserProfile::GetID() const
{
    return this->id;
}

std::string_view UserProfile::GetName() const
{
    return this->managerName;
}
