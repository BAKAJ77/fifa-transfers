#include <states/new_season_setup.h>
#include <states/continue_game.h>

#include <serialization/save_data.h>
#include <util/random_engine.h>
#include <util/timestamp.h>
#include <util/globals.h>

void NewSeasonSetup::Init()
{
    // Initialize member variables
    this->opacity = 0.0f;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Update database for the start of new season
    this->UpdateCurrentSaveDataState();

    for (UserProfile& user : SaveData::GetInstance().GetUsers())
    {
        this->UpdateUserClubsState(user);
        this->UpdateUserCompetitionStats(user);
    }
}

void NewSeasonSetup::Destroy() {}

void NewSeasonSetup::UpdateCurrentSaveDataState() const
{
    // Update the save's current year
    SaveData::GetInstance().SetCurrentYear(SaveData::GetInstance().GetCurrentYear() + 1);

    // Update the current league being played in this save
    const League* currentLeague = SaveData::GetInstance().GetCurrentLeague();
    
    // Tally up the amount of users in the auto promotion, playoffs and relegation spots
    // Also take note if a user won the league title or playoffs
    int totalUsersInAutoPromotion = 0, totalUsersInPlayoffs = 0, totalUsersInRelegation = 0;
    bool leagueTitleWasWon = false, playoffTitleWasWon = false;

    for (const UserProfile& user : SaveData::GetInstance().GetUsers())
    {
        for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
        {
            if (compStats.compID == currentLeague->GetID()) // We only want the stats for the LEAGUE competition
            {
                if (compStats.seasonEndPosition == 1)
                    leagueTitleWasWon = true;
                else if (compStats.wonPlayoffs)
                    playoffTitleWasWon = true;

                if (compStats.seasonEndPosition <= currentLeague->GetAutoPromotionThreshold())
                    ++totalUsersInAutoPromotion;
                else if (compStats.seasonEndPosition <= currentLeague->GetPlayoffsThreshold())
                    ++totalUsersInPlayoffs;
                else if (compStats.seasonEndPosition >= currentLeague->GetRelegationThreshold())
                    ++totalUsersInRelegation;
            }
        }
    }

    // The users get upgraded to a higher tier league (assuming they aren't already in the highest tier league) if:
    // [+] A user wins the title.
    // [+] All the users finish in the auto promotion spots.
    // [+] All users either finish in the auto promotion spots or win the playoffs.
    // 
    // On the other hand, the users get downgraded to a lower tier league (assuming they aren't already in the lowest tier league) if:
    // [-] If the user finishes in a relegation spot (This only applies to solo user saves).
    // [-] If both users finish in a relegation spot (This only applies to duo user saves).
    // [-] If at least half of the users in the save finish in a relegation spot (Applies to saves with 3+ users).

    const int totalUsersInSave = (int)SaveData::GetInstance().GetUsers().size();

    if (leagueTitleWasWon || (totalUsersInAutoPromotion == totalUsersInSave) || (totalUsersInAutoPromotion == (totalUsersInSave - 1) && playoffTitleWasWon))
    {
        // Fetch the league in the same nation which is a tier higher, then set it as the save's current league
        for (League& league : SaveData::GetInstance().GetLeagueDatabase())
        {
            if (league.GetNation() == currentLeague->GetNation() && league.GetTier() == (currentLeague->GetTier() - 1))
            {
                SaveData::GetInstance().SetCurrentLeague(&league);
                break;
            }
        }
    }
    else if ((totalUsersInSave == 1 && totalUsersInRelegation == 1) || (totalUsersInSave == 2 && totalUsersInRelegation == 2) || 
        (totalUsersInRelegation >= std::ceil((float)totalUsersInSave / 2.0f)))
    {
        // Fetch the league in the same nation which is a tier lower, then set it as the save's current league
        for (League& league : SaveData::GetInstance().GetLeagueDatabase())
        {
            if (league.GetNation() == currentLeague->GetNation() && league.GetTier() == (currentLeague->GetTier() + 1))
            {
                SaveData::GetInstance().SetCurrentLeague(&league);
                break;
            }
        }
    }

    // Update the league ID linked to the user's clubs
    for (UserProfile& user : SaveData::GetInstance().GetUsers())
        user.GetClub()->SetLeague(SaveData::GetInstance().GetCurrentLeague()->GetID());

    // Add new competition stats tracker for the new current league the users are playing in
    for (UserProfile& user : SaveData::GetInstance().GetUsers())
    {
        // Make sure the competition stats tracker for the new current league doesn't already exist
        bool alreadyTrackingLeague = false;
        for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
        {
            if (compStats.compID == SaveData::GetInstance().GetCurrentLeague()->GetID())
            {
                alreadyTrackingLeague = true;
                break;
            }
        }

        if (!alreadyTrackingLeague)
        {
            user.GetCompetitionData().insert(user.GetCompetitionData().begin(), { (uint16_t)(user.GetCompetitionData().size() + 1), 
                SaveData::GetInstance().GetCurrentLeague()->GetID() });

            // Add new competition stats tracker for any new cup competitions the users get to play in
            for (const League::CompetitionLink& cupCompLink : SaveData::GetInstance().GetCurrentLeague()->GetLinkedCompetitions())
            {
                // Make sure the competition stats tracker for the cup competition doesn't already exist
                bool alreadyTrackingCup = false;
                for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
                {
                    if (compStats.compID == cupCompLink.competitionID)
                    {
                        alreadyTrackingCup = true;
                        break;
                    }
                }

                if (!alreadyTrackingCup)
                    user.GetCompetitionData().push_back({ (uint16_t)(user.GetCompetitionData().size() + 1), cupCompLink.competitionID });
            }
        }
    }

    // Update the contracts of players in AI clubs
    for (Club& club : SaveData::GetInstance().GetClubDatabase())
    {
        // Only do this operation for players in AI CLUBS
        bool isAIClub = true;
        for (const UserProfile& user : SaveData::GetInstance().GetUsers())
        {
            if (club.GetID() == user.GetClub()->GetID())
            {
                isAIClub = false;
                break;
            }
        }

        if (isAIClub)
        {
            // If a player's contract length is at 0 then reset it back to 5 years
            for (Player* player : club.GetPlayers())
            {
                if ((player->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) <= 0)
                    player->SetExpiryYear(SaveData::GetInstance().GetCurrentYear() + 5);
            }
        }
    }
}

void NewSeasonSetup::UpdateUserCompetitionStats(UserProfile& user) const
{
    for (UserProfile::CompetitionData& compStats : user.GetCompetitionData())
    {
        compStats.seasonEndPosition = 0;
        compStats.wonPlayoffs = false;

        // Update the 'most[x]' stats
        if (compStats.currentScored > compStats.mostScored)
            compStats.mostScored = compStats.currentScored;

        if (compStats.currentConceded > compStats.mostConceded)
            compStats.mostConceded = compStats.currentConceded;

        if (compStats.currentWins > compStats.mostWins)
            compStats.mostWins = compStats.currentWins;

        if (compStats.currentDraws > compStats.mostDraws)
            compStats.mostDraws = compStats.currentDraws;

        if (compStats.currentLosses > compStats.mostLosses)
            compStats.mostLosses = compStats.currentLosses;

        // Reset the 'current[x]' stats to 0
        compStats.currentScored = 0;
        compStats.currentConceded = 0;
        compStats.currentWins = 0;
        compStats.currentDraws = 0;
        compStats.currentLosses = 0;
    }
}

void NewSeasonSetup::UpdateUserClubsState(UserProfile& user) const
{
    // Reset the training staff levels back to 0
    user.GetClub()->GetTrainingStaff(Club::StaffType::GOALKEEPING).level = 0;
    user.GetClub()->GetTrainingStaff(Club::StaffType::DEFENCE).level = 0;
    user.GetClub()->GetTrainingStaff(Club::StaffType::MIDFIELD).level = 0;
    user.GetClub()->GetTrainingStaff(Club::StaffType::ATTACK).level = 0;

    // Clear all the general messages in the club's inbox
    user.GetClub()->GetGeneralMessages().clear();
    
    // Generate new objectives for the user's club
    user.GetClub()->GenerateObjectives();

    for (Player* player : user.GetClub()->GetPlayers())
    {
        // For each player who's contract is running low (i.e 1 year left), send the user a general message letting him know
        if ((player->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) == 1)
        {
            user.GetClub()->GetGeneralMessages().push_back({ std::string(player->GetName()) +
                " only has 1 year left on his contract. We suggest you renew his contract if you don't want him to leave on a free." });
        }

        if ((player->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) == 0)
        {
            // Generate a new contract length for the player
            const int contractLength = RandomEngine::GetInstance().GenerateRandom<int>(2, 5);
            player->SetExpiryYear(player->GetExpiryYear() + contractLength);

            // If the user's club's squad is at the minimum limit (i.e has only 16 players) then renew every contract which has ended
            // and let the user know that this has occurred via general messages.
            if (user.GetClub()->GetPlayers().size() <= Globals::minSquadSize)
            {
                user.GetClub()->GetGeneralMessages().push_back({ "We've had to renew " + std::string(player->GetName()) + " on a " + std::to_string(contractLength) +
                    " year contract, due to your squad size being at the minimum limit, which is 16 players." });
            }
            else // Release the player to a random club
            {
                bool suitableAIClubFound = false;

                while (!suitableAIClubFound)
                {
                    // Choose random club from the save's database
                    const int randomClubIndex = RandomEngine::GetInstance().GenerateRandom<int>(0, (int)SaveData::GetInstance().GetClubDatabase().size() - 1);
                    Club* aiClub = &SaveData::GetInstance().GetClubDatabase()[randomClubIndex];

                    // Make sure the chosen club isn't controlled by a user
                    bool clubControlledByUser = false;
                    for (UserProfile& user : SaveData::GetInstance().GetUsers())
                    {
                        if (aiClub->GetID() == user.GetClub()->GetID())
                        {
                            clubControlledByUser = true;
                            break;
                        }
                    }

                    // To keep it realistic, make sure the club chosen isn't way too good/bad for the player
                    constexpr int requiredOverallRange = 5;
                    if (!clubControlledByUser)
                    {
                        if (player->GetOverall() >= 60)
                        {
                            if (aiClub->GetAverageOverall() >= player->GetOverall() - requiredOverallRange &&
                                aiClub->GetAverageOverall() <= player->GetOverall() + requiredOverallRange)
                            {
                                suitableAIClubFound = true;
                            }
                        }
                        else
                        {
                            if (aiClub->GetAverageOverall() <= 65)
                                suitableAIClubFound = true;
                        }
                    }

                    // The AI club chosen must have enough space in their squad for the player
                    if (aiClub->GetPlayers().size() >= Globals::maxSquadSize)
                        suitableAIClubFound = false;

                    if (suitableAIClubFound)
                    {
                        // Move the player from the user's club to the AI club
                        aiClub->AddPlayer(player);
                        user.GetClub()->RemovePlayer(player);

                        // Update the user's club wage budget
                        user.GetClub()->SetWageBudget(user.GetClub()->GetWageBudget() + player->GetWage());

                        // Send general message to user to let him know that the player has left the club
                        user.GetClub()->GetGeneralMessages().push_back({ std::string(player->GetName()) + " has signed for " + aiClub->GetName().data() + " on a" +
                            std::to_string(contractLength) + " year deal as a free agent." });

                        // Remove any pending transfer messages involving this player
                        for (Club& club : SaveData::GetInstance().GetClubDatabase())
                        {
                            std::vector<Club::Transfer>& transferInbox = club.GetTransferMessages();

                            for (int index = 0; index < (int)transferInbox.size(); index++)
                            {
                                if (transferInbox[index].playerID == player->GetID())
                                {
                                    transferInbox.erase(transferInbox.begin() + index);
                                    --index;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void NewSeasonSetup::Update(const float& deltaTime) 
{
    static float startupTime = Util::GetSecondsSinceEpoch();
    if ((Util::GetSecondsSinceEpoch() - startupTime) >= 3.0f)
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the fade out effect of the app state text and background
        this->opacity = std::max(this->opacity - (transitionSpeed * deltaTime), 0.0f);
        if (this->opacity == 0.0f)
            this->RollBack(ContinueGame::GetAppState());
    }
}

void NewSeasonSetup::Render() const 
{
    // Render the background
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 1"), { 255, 255, 255, this->opacity });

    // Render the app state text
    const glm::vec2 textSize = Renderer::GetInstance().GetTextSize(this->font, 80, "STARTING NEW SEASON, PLEASE WAIT...");
    Renderer::GetInstance().RenderShadowedText({ 990 - (textSize.x / 2.0f), 540 + (textSize.y / 2.0f) }, { glm::vec3(255), this->opacity }, this->font, 80,
        "STARTING NEW SEASON, PLEASE WAIT...", 5);
}

bool NewSeasonSetup::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the app state text and background
    this->opacity = std::min(this->opacity + (transitionSpeed * deltaTime), 255.0f);
    if (this->opacity == 255.0f)
        return true;

    return false;
}

NewSeasonSetup* NewSeasonSetup::GetAppState()
{
    static NewSeasonSetup appState;
    return &appState;
}
