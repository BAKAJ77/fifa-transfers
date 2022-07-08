#ifndef CUP_GROUP_H
#define CUP_GROUP_H

#include <string>
#include <vector>

class KnockoutCup
{
private:
	uint16_t id;
	std::string name;
	std::vector<std::string> nations, rounds;
	float winnerBonus;
public:
	KnockoutCup();
	KnockoutCup(uint16_t id, const std::string_view& name, const std::vector<std::string>& nations, const std::vector<std::string>& rounds, 
		float winnerBonus);

	~KnockoutCup() = default;

	// Sets the name of the cup competition.
	void SetName(const std::string_view& name);

	// Sets the nations that can participate in the cup competition.
	void SetNations(const std::vector<std::string>& nations);

	// Sets the rounds in the cup competition.
	void SetRounds(const std::vector<std::string>& rounds);

	// Sets the winner bonus of the cup competition.
	void SetWinnerBonus(float multiplier);

	// Returns the ID of the cup competition.
	const uint16_t& GetID() const;

	// Returns the name of the cup competition.
	std::string_view GetName() const;

	// Returns the nations that can participate in the cup competition.
	const std::vector<std::string>& GetNations() const;

	// Returns the rounds in the cup competition.
	const std::vector<std::string>& GetRounds() const;

	// Returns the winner bonus of the cup competition.
	const float& GetWinnerBonus() const;
};

#endif