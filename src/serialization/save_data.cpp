#include <serialization/save_data.h>

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
