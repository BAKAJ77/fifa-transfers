#include <serialization/user_profile.h>

UserProfile::UserProfile() :
    club(nullptr)
{}

UserProfile::UserProfile(const std::string_view& name, Club& club) :
    managerName(name), club(&club)
{}

void UserProfile::SetName(const std::string_view& name)
{
    this->managerName = name;
}

void UserProfile::SetClub(Club& club)
{
    this->club = &club;
}

void UserProfile::AddCompetitionData(const CompetitionData& comp)
{
    this->compData.emplace_back(comp);
}

std::vector<UserProfile::CompetitionData>& UserProfile::GetCompetitionData()
{
    return this->compData;
}

Club* UserProfile::GetClub()
{
    return this->club;
}

std::string_view UserProfile::GetName() const
{
    return this->managerName;
}
