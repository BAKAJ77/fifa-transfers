#ifndef NEW_SEASON_SETUP_H
#define NEW_SEASON_SETUP_H

#include <core/application_state.h>
#include <interface/user_interface.h>
#include <serialization/user_profile.h>

class NewSeasonSetup : public AppState
{
private:
	FontPtr font;
	float opacity;
private:
	// Updates the current state of the save data and the databases.
	void UpdateCurrentSaveDataState() const;

	// Updates the given user's competition stats for the new season.
	void UpdateUserCompetitionStats(UserProfile& user) const;

	// Updates the given user's club state.
	// This includes clearing the club's general messages, resetting training staff levels etc.
	void UpdateUserClubsState(UserProfile& user) const;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static NewSeasonSetup* GetAppState();
};

#endif
