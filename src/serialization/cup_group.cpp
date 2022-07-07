#include <serialization/cup_group.h>

KnockoutCup::KnockoutCup() :
    id(0), winnerBonus(0.0f)
{}

KnockoutCup::KnockoutCup(uint16_t id, const std::string_view& name, const std::string_view& nation, const std::vector<std::string>& rounds, 
    float winnerBonus) :
    id(id), name(name), nation(nation), rounds(rounds), winnerBonus(winnerBonus)
{}

void KnockoutCup::SetName(const std::string_view& name)
{
    this->name = name;
}

void KnockoutCup::SetNation(const std::string_view& nation)
{
    this->nation = nation;
}

void KnockoutCup::SetRounds(const std::vector<std::string>& rounds)
{
    this->rounds = rounds;
}

void KnockoutCup::SetWinnerBonus(float multiplier)
{
    this->winnerBonus = multiplier;
}

const uint16_t& KnockoutCup::GetID() const
{
    return this->id;
}

std::string_view KnockoutCup::GetName() const
{
    return this->name;
}

std::string_view KnockoutCup::GetNation() const
{
    return this->nation;
}

const std::vector<std::string>& KnockoutCup::GetRounds() const
{
    return this->rounds;
}

const float& KnockoutCup::GetWinnerBonus() const
{
    return this->winnerBonus;
}
