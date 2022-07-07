#ifndef LEAGUE_GROUP_H
#define LEAGUE_GROUP_H

#include <serialization/club_entity.h>
#include <vector>

class League
{
private:
	std::string name, nation;
	uint16_t id, tier;
	int championsLeague, europaLeague, conferenceLeague, autoPromotion, playoffs, relegation;
	float titleBonus;

	std::vector<Club*> clubs;
public:
	League();
	League(const std::string_view& name, const std::string_view& nation, uint16_t id, uint16_t tier, int championsLeague, int europaLeague,
		int conferenceLeague, int autoPromotion, int playoffs, int relegation, float titleBonus, const std::vector<Club*> clubs);

	~League() = default;

	// Sets the name of the league.
	void SetName(const std::string_view& name);

	// Sets the nation of the league.
	void SetNation(const std::string_view& nation);

	// Sets the tier of the league.
	void SetTier(uint16_t tier);

	// Set the threshold for clubs in the league to qualify for the Champions League.
	void SetChampionsLeagueThreshold(int threshold);

	// Set the threshold for clubs in the league to qualify for the Europa League.
	void SetEuropaLeagueThreshold(int threshold);

	// Set the threshold for clubs in the league to qualify for the Conference League.
	void SetConferenceLeagueThreshold(int threshold);

	// Set the threshold for clubs in the league to qualify for Automatic Promotion.
	void SetAutoPromotionThreshold(int threshold);

	// Set the threshold for clubs in the league to qualify for the Playoffs.
	void SetPlayoffsThreshold(int threshold);

	// Set the threshold for clubs in the league to qualify for Relegation.
	void SetRelegationThreshold(int threshold);

	// Sets the title bonus multiplier of the league.
	void SetTitleBonus(float multiplier);

	// Adds the given club to the league.
	void AddClub(Club* club);

	// Removes the given club from the league.
	void RemoveClub(Club* club);

	// Returns the clubs in the league.
	std::vector<Club*>& GetClubs();

	// Returns the name of the league.
	std::string_view GetName() const;

	// Returns the nation the league resides in.
	std::string_view GetNation() const;

	// Returns the ID of the league.
	const uint16_t& GetID() const;

	// Returns the tier of the league.
	const uint16_t& GetTier() const;

	// Returns the threshold for clubs in the league to qualify for the Champions League.
	const int& GetChampionsLeagueThreshold() const;

	// Returns the threshold for clubs in the league to qualify for the Europa League.
	const int& GetEuropaLeagueThreshold() const;

	// Returns the threshold for clubs in the league to qualify for the Conference League.
	const int& GetConferenceLeagueThreshold() const;

	// Returns the threshold for clubs in the league to qualify for Automatic Promotion.
	const int& GetAutoPromotionThreshold() const;

	// Returns the threshold for clubs in the league to qualify for Playoffs.
	const int& GetPlayoffsThreshold() const;

	// Returns the threshold for clubs in the league to qualify for Relegation.
	const int& GetRelegationThreshold() const;
};

#endif
