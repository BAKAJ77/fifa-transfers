#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include <string>

class SaveData
{
public:
	enum class GrowthSystemType
	{
		OVERALL_RATING,
		SKILL_POINTS
	};
private:
	std::string name;
	uint8_t playerCount;
	GrowthSystemType growthSystemType;
public:
	SaveData();
	SaveData(const SaveData& other) = delete;
	SaveData(SaveData&& temp) noexcept = delete;

	~SaveData() = default;

	// Sets the name of the save.
	void SetSaveName(const std::string_view& name);

	// Sets the number of users in the save.
	void SetPlayerCount(uint8_t count);

	// Sets the type of growth system used in the save.
	void SetGrowthSystem(GrowthSystemType type);

	// Returns the name of the save.
	std::string_view GetName() const;

	// Returns the number of users in the save.
	const uint8_t& GetPlayerCount() const;

	// Returns the type of growth system used in the save.
	const GrowthSystemType& GetGrowthSystemType() const;

	// Returns singleton instance object of this class.
	static SaveData& GetInstance();
};

#endif
