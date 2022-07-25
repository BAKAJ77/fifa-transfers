#include <serialization/cup_group.h>

KnockoutCup::KnockoutCup() :
    id(0), winnerBonus(0.0f), tier(0)
{}

KnockoutCup::KnockoutCup(uint16_t id, const std::string_view& name, const std::string_view& region, const std::vector<std::string>& rounds, 
    float winnerBonus, int tier) :
    id(id), name(name), rounds(rounds), winnerBonus(winnerBonus), region(region), tier(tier)
{}

void KnockoutCup::SetName(const std::string_view& name)
{
    this->name = name;
}

void KnockoutCup::SetRounds(const std::vector<std::string>& rounds)
{
    this->rounds = rounds;
}

void KnockoutCup::SetWinnerBonus(float multiplier)
{
    this->winnerBonus = multiplier;
}

void KnockoutCup::SetTier(int tier)
{
    this->tier = tier;
}

const uint16_t& KnockoutCup::GetID() const
{
    return this->id;
}

std::string_view KnockoutCup::GetName() const
{
    return this->name;
}

const std::vector<std::string>& KnockoutCup::GetRounds() const
{
    return this->rounds;
}

const float& KnockoutCup::GetWinnerBonus() const
{
    return this->winnerBonus;
}

const int& KnockoutCup::GetTier() const
{
    return this->tier;
}