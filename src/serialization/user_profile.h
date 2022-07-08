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
		uint16_t id, compID;

		int currentScored = 0, currentConceded = 0, currentWins = 0, currentDraws = 0, currentLosses = 0;
		int	totalScored = 0, totalConceded = 0, totalWins = 0, totalDraws = 0, totalLosses = 0;
		int	mostScored = 0, mostConceded = 0, mostWins = 0, mostDraws = 0, mostLosses = 0;

		int titlesWon = 0;
	};
private:
	uint16_t id;
	std::string managerName;
	std::vector<CompetitionData> compData;
	Club* club;
public:
	UserProfile();
	UserProfile(uint16_t id, const std::string_view& name, Club& club);

	~UserProfile() = default;

	// Sets the managerial name of the user.
	void SetName(const std::string_view& name);

	// Sets the club which the user is managing.
	void SetClub(Club& club);

	// Adds the competition to the profile records so the user's performance can be tracked.
	void AddCompetitionData(const CompetitionData& comp);

	// Returns data on how the user has performed in competitions.
	std::vector<CompetitionData>& GetCompetitionData();

	// Returns data on how the user has performed in competitions.
	const std::vector<CompetitionData>& GetCompetitionData() const;

	// Returns the club the user is managing.
	Club* GetClub();

	// Returns the club the user is managing.
	const Club* GetClub() const;

	// Returns the ID of the user profile.
	const uint16_t& GetID() const;

	// Returns the managerial name of the user.
	std::string_view GetName() const;
};

#endif