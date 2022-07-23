#include <serialization/player_entity.h>

Player::Player() :
    id(0), clubID(0), positionID(0), age(0), overall(0), potential(0), value(0), wage(0), releaseClause(0), expiryYear(0), transferListed(false), 
    transfersBlocked(false)
{}

Player::Player(const std::string_view& name, const std::string_view& nation, const std::string_view& preferredFoot, uint16_t id, uint16_t clubID,
    uint16_t positionID, int age, int overall, int potential, int value, int wage, int releaseClause, int expiryYear, bool transferListed, 
    bool transfersBlocked) :
    name(name), nation(nation), preferredFoot(preferredFoot), id(id), clubID(clubID), positionID(positionID), age(age), overall(overall), 
    potential(potential), value(value), wage(wage), releaseClause(releaseClause), expiryYear(expiryYear), transferListed(transferListed), 
    transfersBlocked(transfersBlocked)
{}

void Player::SetName(const std::string_view& name)
{
    this->name = name;
}

void Player::SetNation(const std::string_view& nation)
{
    this->nation = nation;
}

void Player::SetPreferredFoot(const std::string_view& foot)
{
    this->preferredFoot = foot;
}

void Player::SetClub(uint16_t id)
{
    this->clubID = id;
}

void Player::SetPosition(uint16_t id)
{
    this->positionID = id;
}

void Player::SetAge(int age)
{
    this->age = age;
}

void Player::SetOverall(int overall)
{
    this->overall = overall;
}

void Player::SetPotential(int potential)
{
    this->potential = potential;
}

void Player::SetValue(int value)
{
    this->value = value;
}

void Player::SetWage(int wage)
{
    this->wage = wage;
}

void Player::SetReleaseClause(int amount)
{
    this->releaseClause = amount;
}

void Player::SetExpiryYear(int year)
{
    this->expiryYear = year;
}

void Player::SetTransferListed(bool listed)
{
    this->transferListed = listed;
}

void Player::SetTransfersBlocked(bool block)
{
    this->transfersBlocked = block;
}

std::string_view Player::GetName() const
{
    return this->name;
}

std::string_view Player::GetNation() const
{
    return this->nation;
}

std::string_view Player::GetPreferredFoot() const
{
    return this->preferredFoot;
}

uint16_t Player::GetID() const
{
    return this->id;
}

uint16_t Player::GetClub() const
{
    return this->clubID;
}

uint16_t Player::GetPosition() const
{
    return this->positionID;
}

int Player::GetAge() const
{
    return this->age;
}

int Player::GetOverall() const
{
    return this->overall;
}

int Player::GetPotential() const
{
    return this->potential;
}

int Player::GetValue() const
{
    return this->value;
}

int Player::GetWage() const
{
    return this->wage;
}

int Player::GetReleaseClause() const
{
    return this->releaseClause;
}

int Player::GetExpiryYear() const
{
    return this->expiryYear;
}

bool Player::GetTransferListed() const
{
    return this->transferListed;
}

bool Player::GetTransfersBlocked() const
{
    return this->transfersBlocked;
}