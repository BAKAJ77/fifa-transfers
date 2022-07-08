#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include <serialization/club_entity.h>
#include <string>
#include <vector>

class UserProfile
{
public:
	struct CompetitionData
	{
		uint16_t compID;

		int currentScored, currentConceded, currentWins, currentDraws, currentLosses;
		int	totalScored, totalConceded, totalWins, totalDraws, totalLosses;
		int	mostScored, mostConceded, mostWins, mostDraws, mostLosses;

		int titlesWon;
	};
private:
	std::string managerName;
	std::vector<CompetitionData> compData;
	Club* club;
public:
	UserProfile();
	UserProfile(const std::string_view& name, Club& club);

	~UserProfile() = default;

	// Sets the managerial name of the user.
	void SetName(const std::string_view& name);

	// Sets the club which the user is managing.
	void SetClub(Club& club);

	// Adds the competition to the profile records so the user's performance can be tracked.
	void AddCompetitionData(const CompetitionData& comp);

	// Returns data on how the user has performed in competitions.
	std::vector<CompetitionData>& GetCompetitionData();

	// Returns the club the user is managing.
	Club* GetClub();

	// Returns the managerial name of the user.
	std::string_view GetName() const;
};

#endif