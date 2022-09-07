#ifndef OBJECTIVES_H
#define OBJECTIVES_H

#include <core/application_state.h>
#include <graphics/texture_loader.h>
#include <interface/user_interface.h>

#include <serialization/club_entity.h>
#include <serialization/user_profile.h>

class Objectives : public AppState
{
private:
	UserInterface userInterface;
	TextureBuffer2DPtr checkmarkCircleTex, crossCircleTex, unknownCircleTex;
	FontPtr font;

	UserProfile* focusedUser;
	bool startedFromMyClubMenu, exitState;
private:
	void RenderObjectiveStatusIndicator(const Club::Objective& objective, float yPos, bool isLeagueObjective) const;
protected:
	void Init() override;
	void Destroy() override;

	void Update(const float& deltaTime) override;
	void Render() const override;

	bool OnStartupTransitionUpdate(const float deltaTime) override;
public:
	static Objectives* GetAppState();

	// Sets the user who's objectives will be displayed.
	void SetUserProfile(UserProfile* user, bool startedFromMyClubMenu = true);
};

#endif