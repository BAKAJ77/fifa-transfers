#ifndef LEAGUE_GROUP_H
#define LEAGUE_GROUP_H

#include <serialization/club_entity.h>
#include <vector>

class League
{
public:
	struct CompetitionLink
	{
		uint16_t competitionID;
		std::vector<uint8_t> qualifyingTablePositions;
	};
private:
	std::string name, nation;
	uint16_t id, tier;
	int autoPromotion, playoffs, relegation, titleBonus;

	std::vector<CompetitionLink> linkedCompetitions;
	std::vector<Club*> clubs;
	bool supported;
public:
	League();
	League(const std::string_view& name, const std::string_view& nation, uint16_t id, uint16_t tier, int autoPromotion, int playoffs, int relegation, 
		int titleBonus, const std::vector<CompetitionLink>& linkedComps, const std::vector<Club*> clubs, bool supported);

	~League() = default;

	// Sets the name of the league.
	void SetName(const std::string_view& name);

	// Sets the nation of the league.
	void SetNation(const std::string_view& nation);

	// Sets the tier of the league.
	void SetTier(uint16_t tier);

	// Set the threshold for clubs in the league to qualify for Automatic Promotion.
	void SetAutoPromotionThreshold(int threshold);

	// Set the threshold for clubs in the league to qualify for the Playoffs.
	void SetPlayoffsThreshold(int threshold);

	// Set the threshold for clubs in the league to qualify for Relegation.
	void SetRelegationThreshold(int threshold);

	// Sets the bonus cash the title winner of the league gets.
	void SetTitleBonus(int amount);

	// Adds the given club to the league.
	void AddClub(Club* club);

	// Removes the given club from the league.
	void RemoveClub(Club* club);

	// Returns the average overall of the clubs in the league.
	int GetAverageOverall() const;

	// Returns the clubs in the league.
	std::vector<Club*>& GetClubs();

	// Returns the clubs in the league.
	const std::vector<Club*>& GetClubs() const;

	// Returns the name of the league.
	std::string_view GetName() const;

	// Returns the nation the league resides in.
	std::string_view GetNation() const;

	// Returns the ID of the league.
	const uint16_t& GetID() const;

	// Returns the tier of the league.
	const uint16_t& GetTier() const;

	// Returns the bonus cash recieved if you win the league title.
	const int& GetTitleBonus() const;

	// Returns the threshold for clubs in the league to qualify for Automatic Promotion.
	const int& GetAutoPromotionThreshold() const;

	// Returns the threshold for clubs in the league to qualify for Playoffs.
	const int& GetPlayoffsThreshold() const;

	// Returns the threshold for clubs in the league to qualify for Relegation.
	const int& GetRelegationThreshold() const;

	// Returns the domestic competitions that clubs in the league can participate in and international competitions that clubs 
	// in the league can qualify for e.g. UCL, UEL etc.
	const std::vector<CompetitionLink>& GetLinkedCompetitions() const;

	// Returns TRUE if the league is playable/supported in the game.
	bool IsSupported() const;
};

#endif
