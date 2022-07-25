#ifndef CUP_GROUP_H
#define CUP_GROUP_H

#include <string>
#include <vector>

class KnockoutCup
{
private:
	uint16_t id;
	std::string name, region;
	std::vector<std::string> rounds;
	float winnerBonus;
	int tier;
public:
	KnockoutCup();
	KnockoutCup(uint16_t id, const std::string_view& name, const std::string_view& region, const std::vector<std::string>& rounds, float winnerBonus, 
		int tier);

	~KnockoutCup() = default;

	// Sets the name of the cup competition.
	void SetName(const std::string_view& name);

	// Sets the rounds in the cup competition.
	void SetRounds(const std::vector<std::string>& rounds);

	// Sets the winner bonus of the cup competition.
	void SetWinnerBonus(float multiplier);

	// Sets the tier of the cup competition.
	void SetTier(int tier);

	// Returns the ID of the cup competition.
	const uint16_t& GetID() const;

	// Returns the name of the cup competition.
	std::string_view GetName() const;

	// Returns the rounds in the cup competition.
	const std::vector<std::string>& GetRounds() const;

	// Returns the winner bonus of the cup competition.
	const float& GetWinnerBonus() const;

	// Returns the tier of the cup competition.
	const int& GetTier() const;
};

#endif