#include <states/record_competition.h>
#include <states/end_competition.h>
#include <states/main_game.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/random_engine.h>
#include <util/data_manip.h>
#include <util/globals.h>

void RecordCompetition::Init()
{
    // Initialize the member variables
    this->exitState = this->completed = this->goalsScoredInvalid = this->goalsConcededInvalid = this->gamesWonInvalid = this->gamesDrawnInvalid =
        this->gamesLostInvalid = this->roundsInvalid = this->wonCupInvalid = this->tablePositionInvalid = this->wonPlayOffsInvalid = false;

    this->userProfileIndex = 0;
    
    EndCompetition::GetAppState()->GetCompetitionSelectionList().GetCurrentSelected() >= 1000 ? // Cup competitions have an ID exceeding 1000
        this->selectedCup = SaveData::GetInstance().GetCup(EndCompetition::GetAppState()->GetCompetitionSelectionList().GetCurrentSelected()) :
        this->selectedCup = nullptr;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddTextField("Goals Scored", TextInputField({ 330, 245 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    this->userInterface.AddTextField("Goals Conceded", TextInputField({ 330, 465 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    
    this->userInterface.AddTextField("Games Won", TextInputField({ 330, 685 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    this->userInterface.AddTextField("Games Drawn", TextInputField({ 330, 905 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    this->userInterface.AddTextField("Games Lost", TextInputField({ 1500, 245 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));

    if (this->selectedCup) 
    {
        this->userInterface.AddDropDown("Rounds", DropDown({ 1500, 465 }, { 600, 75 }));
        for (size_t index = 0; index < this->selectedCup->GetRounds().size(); index++)
            this->userInterface.GetDropDown("Rounds")->AddSelection(this->selectedCup->GetRounds()[index], (int)(index + 1));

        this->userInterface.AddRadioButtonGroup("Won Cup", RadioButtonGroup({ 1225, 665 }, { 50, 50 }, 0.0f));
        this->userInterface.GetRadioButtonGroup("Won Cup")->Add("Yes", 1);
        this->userInterface.GetRadioButtonGroup("Won Cup")->Add("No", 0);
    }
    else
    {
        this->userInterface.AddTextField("Table Position", TextInputField({ 1500, 465 }, { 600, 75 },
            TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));

        this->userInterface.AddRadioButtonGroup("Won Playoffs", RadioButtonGroup({ 1225, 665 }, { 50, 50 }, 0.0f));
        this->userInterface.GetRadioButtonGroup("Won Playoffs")->Add("Yes", 1);
        this->userInterface.GetRadioButtonGroup("Won Playoffs")->Add("No", 0);
    }
}

void RecordCompetition::Destroy() 
{
    this->recordedCompetitionStats.clear();
}

bool RecordCompetition::ValidateInputs()
{
    // Make sure the basic inputs are not empty
    this->goalsScoredInvalid = this->userInterface.GetTextField("Goals Scored")->GetInputtedText().empty();
    this->goalsConcededInvalid = this->userInterface.GetTextField("Goals Conceded")->GetInputtedText().empty();
    this->gamesWonInvalid = this->userInterface.GetTextField("Games Won")->GetInputtedText().empty();
    this->gamesDrawnInvalid = this->userInterface.GetTextField("Games Drawn")->GetInputtedText().empty();
    this->gamesLostInvalid = this->userInterface.GetTextField("Games Lost")->GetInputtedText().empty();

    if (this->selectedCup)
    {
        // Make sure a value has been selected in the rounds drop down and that the amount of users selecting a specific rounds
        // is limited to make sense e.g. only 2 users max can be in the final.
        const int selectedRound = this->userInterface.GetDropDown("Rounds")->GetCurrentSelected();
        if (selectedRound != -1)
        {
            int roundTakenCount = 0;
            for (const CompetitionStats& stats : this->recordedCompetitionStats)
            {
                if (stats.seasonEndPosition == selectedRound)
                    ++roundTakenCount;
            }

            if (selectedRound == this->selectedCup->GetRounds().size() && roundTakenCount == 2) // FINAL ROUND
                this->roundsInvalid = true;
            else if (selectedRound == (this->selectedCup->GetRounds().size() - 1) && roundTakenCount == 4) // SEMI FINAL ROUND
                this->roundsInvalid = true;
            else
            {
                this->roundsInvalid = false;

                // If two users selected that they made it to the final, make sure that one has selected that they won the cup and 
                // other has selected that they did not
                if (selectedRound == this->selectedCup->GetRounds().size())
                {
                    bool otherUserWonCup = false, anotherUserFinalist = false;
                    for (const CompetitionStats& stats : this->recordedCompetitionStats)
                    {
                        if (stats.seasonEndPosition == selectedRound)
                        {
                            otherUserWonCup = stats.wonCup;
                            anotherUserFinalist = true;
                            break;
                        }
                    }

                    if (this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected() == -1)
                    {
                        this->wonCupInvalid = true;
                    }
                    else if (anotherUserFinalist && ((otherUserWonCup && (bool)this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected()) ||
                        (!otherUserWonCup && !(bool)this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected())))
                    {
                        this->wonCupInvalid = true;
                    }
                    else
                        this->wonCupInvalid = false;
                }
            }
        }
        else
            this->roundsInvalid = true;
    }
    else
    {
        // Make sure the table position input is not empty, is within valid bounds according to the amount of teams in the league and
        // is not already taken by another user.
        if (!this->userInterface.GetTextField("Table Position")->GetInputtedText().empty())
        {
            const uint16_t tablePosition = (uint16_t)std::stoi(this->userInterface.GetTextField("Table Position")->GetInputtedText());

            if (tablePosition < 1 || tablePosition > SaveData::GetInstance().GetCurrentLeague()->GetClubs().size())
            {
                this->tablePositionInvalid = true;
            }
            else
            {
                bool tablePositionTaken = false;
                for (const CompetitionStats& stats : this->recordedCompetitionStats)
                {
                    if (stats.seasonEndPosition == tablePosition)
                    {
                        tablePositionTaken = true;
                        break;
                    }
                }

                this->tablePositionInvalid = tablePositionTaken;
            }

            // Make sure the 'Won Playoffs' question choice has been answered
            // Also, make sure that no more than 1 user, who has finished in the playoffs spot, have selected that they won the playoff trophy
            if (tablePosition > SaveData::GetInstance().GetCurrentLeague()->GetAutoPromotionThreshold() &&
                tablePosition <= SaveData::GetInstance().GetCurrentLeague()->GetPlayoffsThreshold())
            {
                if (this->userInterface.GetRadioButtonGroup("Won Playoffs")->GetSelected() == -1)
                {
                    this->wonPlayOffsInvalid = true;
                }
                else if (this->userInterface.GetRadioButtonGroup("Won Playoffs")->GetSelected() == 1)
                {
                    // Make sure no other user has selected that they've won the playoffs
                    bool anotherUserWonPlayoffs = false;
                    for (const CompetitionStats& stats : this->recordedCompetitionStats)
                    {
                        if (stats.wonPlayoffs)
                        {
                            anotherUserWonPlayoffs = true;
                            break;
                        }
                    }

                    this->wonPlayOffsInvalid = anotherUserWonPlayoffs;
                }
                else
                    this->wonPlayOffsInvalid = false;
            }
            else
                this->wonPlayOffsInvalid = false;
        }
        else
            this->tablePositionInvalid = true;
    }

    return !this->goalsScoredInvalid && !this->goalsConcededInvalid && !this->gamesWonInvalid && !this->gamesDrawnInvalid && !this->gamesLostInvalid &&
        !this->roundsInvalid && !this->wonCupInvalid && !this->tablePositionInvalid && !this->wonPlayOffsInvalid;
}

void RecordCompetition::ClearAllInputs()
{
    this->userInterface.GetTextField("Goals Scored")->Clear();
    this->userInterface.GetTextField("Goals Conceded")->Clear();
    this->userInterface.GetTextField("Games Won")->Clear();
    this->userInterface.GetTextField("Games Drawn")->Clear();
    this->userInterface.GetTextField("Games Lost")->Clear();

    if (this->selectedCup)
    {
        this->userInterface.GetDropDown("Rounds")->Reset();
        this->userInterface.GetRadioButtonGroup("Won Cup")->Reset();
    }
    else
    {
        this->userInterface.GetTextField("Table Position")->Clear();
        this->userInterface.GetRadioButtonGroup("Won Playoffs")->Reset();
    }
}

void RecordCompetition::GenerateAIOutboundTransfers()
{
    for (UserProfile& user : SaveData::GetInstance().GetUsers())
    {
        for (Player* player : user.GetClub()->GetPlayers())
        {
            // Ensure the user has enough players in their squad in order to be able to sell
            if ((player->GetPosition() == 0 && user.GetClub()->GetTotalGoalkeepers() > Globals::minGoalkeepers) ||
                (player->GetPosition() > 0 && user.GetClub()->GetTotalOutfielders() > Globals::minOutfielders))
            {
                // Simple algorithm for AI deciding whether to send an offer for the player
                int generatedWeight = RandomEngine::GetInstance().GenerateRandom<int>(0, 100);

                if (player->GetTransferListed())
                    generatedWeight *= (int)(((float)(player->GetOverall() + player->GetPotential()) / 10.0f) * 3.5f);
                else
                    generatedWeight *= (int)((float)(player->GetOverall() + player->GetPotential()) / 10.0f);

                if (generatedWeight >= 1150)
                {
                    // Simple algorithm to decide the amount willing to be bidded for the player
                    const int min = (int)(std::floor((float)(player->GetValue()) / 2.0f));
                    const int max = (int)(std::ceil((float)(player->GetValue()) *
                        std::clamp((float)(player->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) / 2.0f, 1.0f, 1.5f)));

                    int openingBid = Util::GetTruncatedSFInteger(RandomEngine::GetInstance().GenerateRandom<int>(min, max), 4);

                    // Select a random AI controlled club to make the bid
                    Club* biddingAIClub = nullptr;
                    bool suitableClubFound = false, activeNegotiationCooldownFound = false;

                    // Don't bother bidding for the player if there is an active negotiation cooldown attached to him
                    for (SaveData::NegotiationCooldown& cooldown : SaveData::GetInstance().GetNegotiationCooldowns())
                    {
                        if (cooldown.playerID == player->GetID() && cooldown.clubID == 0)
                        {
                            activeNegotiationCooldownFound = true;
                            break;
                        }
                    }

                    while (!suitableClubFound && !activeNegotiationCooldownFound)
                    {
                        // Choose random club from the save's database
                        const int randomClubIndex = RandomEngine::GetInstance().GenerateRandom<int>(0, (int)SaveData::GetInstance().GetClubDatabase().size() - 1);
                        biddingAIClub = &SaveData::GetInstance().GetClubDatabase()[randomClubIndex];

                        // Make sure the chosen club isn't controlled by a user
                        bool clubControlledByUser = false;
                        for (UserProfile& user : SaveData::GetInstance().GetUsers())
                        {
                            if (biddingAIClub->GetID() == user.GetClub()->GetID())
                            {
                                clubControlledByUser = true;
                                break;
                            }
                        }

                        // Make sure the club hasn't already approached for the player
                        bool alreadyCurrentlyApproachingPlayer = false;
                        for (const auto& transferMsg : biddingAIClub->GetTransferMessages())
                        {
                            if (transferMsg.playerID == player->GetID())
                            {
                                alreadyCurrentlyApproachingPlayer = true;
                                break;
                            }
                        }

                        // To keep it realistic, make sure the club chosen isn't way too good/bad for the player
                        constexpr int requiredOverallRange = 5;
                        if (!clubControlledByUser && !alreadyCurrentlyApproachingPlayer)
                        {
                            if (player->GetOverall() >= 60)
                            {
                                if (biddingAIClub->GetAverageOverall() >= player->GetOverall() - requiredOverallRange &&
                                    biddingAIClub->GetAverageOverall() <= player->GetOverall() + requiredOverallRange)
                                {
                                    suitableClubFound = true;
                                }
                            }
                            else
                            {
                                if (biddingAIClub->GetAverageOverall() <= 65)
                                    suitableClubFound = true;
                            }
                        }

                        // The bidding AI club chosen must have enough space in their squad for the player being bidded for
                        if (biddingAIClub->GetPlayers().size() >= Globals::maxSquadSize)
                            suitableClubFound = false;

                        if (suitableClubFound)
                        {
                            // Slash the amount bidded if the player's wage will consume at least half the club's wage budget
                            if (player->GetWage() >= (biddingAIClub->GetWageBudget() / 2.0f))
                            {
                                openingBid = Util::GetTruncatedSFInteger((int)(openingBid /
                                    (1.5f * ((float)player->GetWage() / (float)biddingAIClub->GetWageBudget()))), 4);
                            }

                            if (openingBid >= player->GetReleaseClause() && player->GetReleaseClause() > 0)
                            {
                                // Send pending release clause activation data to the bidding club. 
                                // Once the expiration ticks reaches 1, a conclusion on the AI release clause activation will be 
                                // computed, so the expiration ticks is set to 2 to allow the user time to decide on renewing the 
                                // contract of the player to prevent the release clause activation deal from being completed. 
                                Club::Transfer pendingReleaseClauseTransfer;
                                pendingReleaseClauseTransfer.biddingClubID = biddingAIClub->GetID();
                                pendingReleaseClauseTransfer.playerID = player->GetID();
                                pendingReleaseClauseTransfer.transferFee = player->GetReleaseClause();
                                pendingReleaseClauseTransfer.expirationTicks = 2;
                                pendingReleaseClauseTransfer.activatedReleaseClause = true;
                                pendingReleaseClauseTransfer.feeAgreed = true;

                                biddingAIClub->GetTransferMessages().emplace_back(pendingReleaseClauseTransfer);
                                user.GetClub()->GetGeneralMessages().push_back({ std::string(biddingAIClub->GetName()) + 
                                    " have triggered the " + Util::GetFormattedCashString(player->GetReleaseClause()) + 
                                    " release clause for " + player->GetName().data() + 
                                    ". If you want to keep him, we suggest you renew his contract now." });
                            }
                            else if (!player->GetTransfersBlocked())
                            {
                                // Send opening transfer offer to seller user's club
                                Club::Transfer openingTransferOffer;
                                openingTransferOffer.biddingClubID = biddingAIClub->GetID();
                                openingTransferOffer.playerID = player->GetID();
                                openingTransferOffer.transferFee = openingBid;
                                openingTransferOffer.expirationTicks = 3;

                                user.GetClub()->GetTransferMessages().emplace_back(openingTransferOffer);
                            }
                        }
                    }
                }
            }
        }
    }
}

void RecordCompetition::HandleAIClubsTransferResponses()
{
    // Iterate through every club in the save's database
    for (Club& club : SaveData::GetInstance().GetClubDatabase())
    {
        // Make sure the club is not controlled by a user
        bool clubControlledByUser = false;
        for (UserProfile& user : SaveData::GetInstance().GetUsers())
        {
            if (club.GetID() == user.GetClub()->GetID())
            {
                clubControlledByUser = true;
                break;
            }
        }

        if (!clubControlledByUser)
        {
            const size_t totalPendingTransferMsgs = club.GetTransferMessages().size();
            for (size_t index = 0; index < totalPendingTransferMsgs; index++)
            {
                const Club::Transfer& transfer = club.GetTransferMessages()[index];
                Player* targettedPlayer = SaveData::GetInstance().GetPlayer(transfer.playerID);

                if (club.GetID() == transfer.biddingClubID) // The AI club is the buyer in this scenario
                {
                    Club* sellerClub = SaveData::GetInstance().GetClub(targettedPlayer->GetClub());

                    // As an absolute caution, make sure both clubs meet the squad size requirements
                    const bool squadSizeRequirementsMet = (club.GetPlayers().size() < Globals::maxSquadSize) &&
                        ((targettedPlayer->GetPosition() == 0 && sellerClub->GetTotalGoalkeepers() > Globals::minGoalkeepers) || 
                        (targettedPlayer->GetPosition() > 0 && sellerClub->GetTotalOutfielders() > Globals::minOutfielders));

                    if (transfer.feeAgreed && squadSizeRequirementsMet)
                    {
                        if (transfer.activatedReleaseClause && transfer.expirationTicks == 1)
                            this->HandleAITransferCompletion(club, *sellerClub, *targettedPlayer, transfer.transferFee, true);
                        else if (!transfer.activatedReleaseClause)
                            this->HandleAITransferCompletion(club, *sellerClub, *targettedPlayer, transfer.transferFee);
                    }
                    else
                    {
                        // Simple algorithm to decide the amount willing to be bidded for the player
                        const int min = (int)(std::floor((float)(targettedPlayer->GetValue()) / 2.0f));
                        const int max = (int)(std::ceil((float)(targettedPlayer->GetValue()) *
                            std::clamp((float)(targettedPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) / 2.0f, 1.0f, 1.5f)));

                        const int willingAmountToBid = Util::GetTruncatedSFInteger(RandomEngine::GetInstance().GenerateRandom<int>(min, max), 4);

                        if ((willingAmountToBid >= transfer.transferFee) && squadSizeRequirementsMet)
                        {
                            // Send general message to the seller user's club indicating that the AI club has agreed to the fee demanded by the user
                            sellerClub->GetGeneralMessages().push_back({ std::string(club.GetName()) + " has agreed to pay your demanded fee of " +
                                Util::GetFormattedCashString(transfer.transferFee) + " for " + targettedPlayer->GetName().data() +
                                ", the player and the club have started negotiating personal terms." });

                            // Push agreed transfer message into the AI clubs transfer inbox (this will be handled at the end of the next competition)
                            Club::Transfer agreedTransfer;
                            agreedTransfer.biddingClubID = transfer.biddingClubID;
                            agreedTransfer.playerID = transfer.playerID;
                            agreedTransfer.transferFee = transfer.transferFee;
                            agreedTransfer.expirationTicks = 3;
                            agreedTransfer.feeAgreed = true;

                            club.GetTransferMessages().emplace_back(agreedTransfer);
                        }
                        else if ((willingAmountToBid >= (transfer.transferFee / 1.75f)) && squadSizeRequirementsMet)
                        {
                            // Send a counter offer, indicating the amount the AI club is willing to pay for the player, to the seller user's club
                            Club::Transfer counterOffer;
                            counterOffer.biddingClubID = transfer.biddingClubID;
                            counterOffer.playerID = transfer.playerID;
                            counterOffer.transferFee = willingAmountToBid;
                            counterOffer.expirationTicks = 3;
                            counterOffer.counterOffer = true;

                            sellerClub->GetTransferMessages().emplace_back(counterOffer);
                        }
                        else
                        {
                            // Send general message to the seller user's club indicating that the AI club has pulled out of negotiations for the player
                            if (squadSizeRequirementsMet)
                            {
                                sellerClub->GetGeneralMessages().push_back({ std::string(club.GetName()) + " have decided to pull out of negotiations for " +
                                    targettedPlayer->GetName().data() });
                            }
                            else
                            {
                                sellerClub->GetGeneralMessages().push_back({ std::string(club.GetName()) + " have decided to pull out of negotiations for " +
                                    targettedPlayer->GetName().data() + " because of squad size requirement related issues." });
                            }
                        }
                    }
                }
                else // The AI club is the seller in this scenario
                {
                    Club* biddingClub = SaveData::GetInstance().GetClub(transfer.biddingClubID);

                    // Simple algorithm to decide the minimum required amount wanted for player
                    const int min = (int)(std::floor((float)targettedPlayer->GetValue() / 1.5f));
                    const int max = (int)(std::ceil((float)targettedPlayer->GetValue() *
                        (1.5f + (((float)targettedPlayer->GetExpiryYear() - (float)SaveData::GetInstance().GetCurrentYear()) / 10.0f))));

                    int minRequiredBid = Util::GetTruncatedSFInteger(RandomEngine::GetInstance().GenerateRandom<int>(min, max), 4);
                    if (targettedPlayer->GetReleaseClause() > 0 && minRequiredBid > targettedPlayer->GetReleaseClause())
                        minRequiredBid = targettedPlayer->GetReleaseClause();

                    if (transfer.transferFee >= minRequiredBid)
                    {
                        // Send response to the bidding user's club that the offer has been accepted
                        Club::Transfer transferResponse;
                        transferResponse.biddingClubID = transfer.biddingClubID;
                        transferResponse.playerID = transfer.playerID;
                        transferResponse.transferFee = transfer.transferFee;
                        transferResponse.expirationTicks = 3;
                        transferResponse.feeAgreed = true;
                        
                        biddingClub->GetTransferMessages().emplace_back(transferResponse);
                    }
                    else if (transfer.transferFee >= (minRequiredBid / 1.75f))
                    {
                        // Send counter response to the bidding user's club
                        Club::Transfer transferResponse;
                        transferResponse.biddingClubID = transfer.biddingClubID;
                        transferResponse.playerID = transfer.playerID;
                        transferResponse.transferFee = minRequiredBid;
                        transferResponse.expirationTicks = 3;

                        biddingClub->GetTransferMessages().emplace_back(transferResponse);
                    }
                    else
                    {
                        biddingClub->GetGeneralMessages().push_back({ std::string(club.GetName()) + " have rejected your approach for " + 
                            targettedPlayer->GetName().data() + " and aren't willing to negotiate any further." });
                    }
                }
            }

            // Erase all the transfer messages in the inbox that were handled
            for (size_t count = 0; count < totalPendingTransferMsgs; count++)
                club.GetTransferMessages().erase(club.GetTransferMessages().begin());
        }
    }
}

void RecordCompetition::HandleAITransferCompletion(Club& buyerClub, Club& sellerClub, Player& player, int transferFee, bool activatedReleaseClause)
{
    // Generate random chance to indicate that the contract negotiations between the player and the AI club was successful
    const float generatedWeight = (float)RandomEngine::GetInstance().GenerateRandom<int>(0, 100);

    if (generatedWeight > ((float)player.GetWage() / (float)buyerClub.GetWageBudget()) * 225.0f) // Contract negotiations was successful
    {
        // Add the paid transfer fee onto the seller user's club transfer budget
        sellerClub.SetTransferBudget(sellerClub.GetTransferBudget() + transferFee);

        // Add the freed wages onto the seller user's club wage budget
        sellerClub.SetWageBudget(sellerClub.GetWageBudget() + player.GetWage());

        // Generate the player's new contract terms
        const int contractLength = RandomEngine::GetInstance().GenerateRandom<int>(player.GetAge() > 26 ? 2 : 3, 5);

        const int min = player.GetWage();
        const int max = (int)(player.GetWage() * 2.25f);
        const int contractWage = Util::GetTruncatedSFInteger(RandomEngine::GetInstance().GenerateRandom<int>(min, max), 3);

        player.SetExpiryYear(SaveData::GetInstance().GetCurrentYear() + contractLength);
        player.SetWage(contractWage);
        player.SetReleaseClause(0);

        // Move the player to his new club
        buyerClub.AddPlayer(&player);
        sellerClub.RemovePlayer(&player);

        // Send general message to the seller user club to notify that the player has been successfully sold
        sellerClub.GetGeneralMessages().push_back({ std::string(buyerClub.GetName()) + " have successfully signed " + 
            player.GetName().data() + " on a " + std::to_string(contractLength) + " year contract for a transfer fee of " + 
            Util::GetFormattedCashString(transferFee) + "." });

        // Erase all transfer messages in every other club's inbox which involve this player
        for (Club& club : SaveData::GetInstance().GetClubDatabase())
        {
            if (club.GetID() != buyerClub.GetID())
            {
                std::vector<Club::Transfer>& transferInbox = club.GetTransferMessages();

                for (int index = 0; index < (int)transferInbox.size(); index++)
                {
                    if (transferInbox[index].playerID == player.GetID())
                    {
                        transferInbox.erase(transferInbox.begin() + index);
                        --index;
                    }
                }
            }
        }

        // Push transfer into the transfer history database
        SaveData::GetInstance().GetTransferHistory().push_back({ player.GetID(), sellerClub.GetID(), buyerClub.GetID(), transferFee });

        // Push negotiation cooldown for all clubs
        SaveData::GetInstance().GetNegotiationCooldowns().push_back({ player.GetID(), 0, SaveData::CooldownType::CONTRACT_NEGOTIATING, 7 });
    }
    else // Contract negotiations was unsuccessful
    {
        sellerClub.GetGeneralMessages().push_back({ std::string(buyerClub.GetName()) +
                " have pulled out of the deal since they couldn't reach an agreement with " + player.GetName().data() });
    }
}

void RecordCompetition::UpdateNegotiationCooldowns()
{
    // Decrease the tick counts of all existing active negotiation cooldowns and if any of them have reached a tick count of 0, then remove them
    std::vector<SaveData::NegotiationCooldown>& negotiationCooldowns = SaveData::GetInstance().GetNegotiationCooldowns();
    for (int index = 0; index < (int)negotiationCooldowns.size(); index++)
    {
        if (--negotiationCooldowns[index].ticksRemaining <= 0)
        {
            negotiationCooldowns.erase(negotiationCooldowns.begin() + index);
            --index;
        }
    }
}

void RecordCompetition::UpdateTransferMessagesTicks()
{
    // Decrease the tick counts of all transfer messages and if any of them have reached a tick count of 0, then remove them
    for (Club& club : SaveData::GetInstance().GetClubDatabase())
    {
        std::vector<Club::Transfer>& clubTransferInbox = club.GetTransferMessages();
        for (int index = 0; index < (int)clubTransferInbox.size(); index++)
        {
            if (--clubTransferInbox[index].expirationTicks <= 0)
            {
                clubTransferInbox.erase(clubTransferInbox.begin() + index);
                --index;
            }
        }
    }
}

void RecordCompetition::UpdateSaveDatabaseState()
{
    this->UpdateTransferMessagesTicks();
    this->UpdateNegotiationCooldowns();
    this->HandleAIClubsTransferResponses();
    this->GenerateAIOutboundTransfers();
}

void RecordCompetition::Update(const float& deltaTime)
{
    if (!this->exitState && !this->completed)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of the buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "CONFIRM" && button->WasClicked() && this->ValidateInputs())
            {
                // Store the user's entered competition stats
                CompetitionStats userStats = { std::stoi(this->userInterface.GetTextField("Goals Scored")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Goals Conceded")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Games Won")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Games Drawn")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Games Lost")->GetInputtedText()) };

                if (this->selectedCup)
                {
                    userStats.seasonEndPosition = this->userInterface.GetDropDown("Rounds")->GetCurrentSelected();
                    if (this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected() == 1)
                        userStats.wonCup = true;
                }
                else
                {
                    userStats.seasonEndPosition = std::stoi(this->userInterface.GetTextField("Table Position")->GetInputtedText());
                    if (this->userInterface.GetRadioButtonGroup("Won Playoffs")->GetSelected() == 1)
                        userStats.wonPlayoffs = true;
                }

                this->recordedCompetitionStats.emplace_back(userStats);

                if (this->userProfileIndex + 1 < SaveData::GetInstance().GetUsers().size())
                {
                    // Update the user profile iteration index
                    ++this->userProfileIndex;

                    // Clear the user interface inputs
                    this->ClearAllInputs();
                }
                else
                {
                    // Update the user profile's competition stats
                    for (size_t index = 0; index < SaveData::GetInstance().GetUsers().size(); index++)
                    {
                        const CompetitionStats& stats = this->recordedCompetitionStats[index];
                        UserProfile* user = &SaveData::GetInstance().GetUsers()[index];

                        for (UserProfile::CompetitionData& compStats : user->GetCompetitionData())
                        {
                            if (compStats.compID == EndCompetition::GetAppState()->GetCompetitionSelectionList().GetCurrentSelected())
                            {
                                compStats.currentScored += stats.scored;
                                compStats.currentConceded += stats.conceded;
                                compStats.currentWins += stats.wins;
                                compStats.currentDraws += stats.draws;
                                compStats.currentLosses += stats.losses;

                                compStats.totalScored += stats.scored;
                                compStats.totalConceded += stats.conceded;
                                compStats.totalWins += stats.wins;
                                compStats.totalDraws += stats.draws;
                                compStats.totalLosses += stats.losses;

                                if (this->selectedCup)
                                {
                                    compStats.seasonEndPosition = stats.seasonEndPosition;

                                    if (stats.wonCup)
                                    {
                                        compStats.seasonEndPosition = (uint16_t)this->selectedCup->GetRounds().size() + 1;
                                        compStats.titlesWon++;
                                    }
                                }
                                else
                                {
                                    compStats.seasonEndPosition = stats.seasonEndPosition;
                                    compStats.wonPlayoffs = stats.wonPlayoffs;

                                    if (compStats.seasonEndPosition == 1)
                                        compStats.titlesWon++;
                                    else if (compStats.wonPlayoffs)
                                    {
                                        compStats.titlesWon++;
                                        compStats.playoffsWon++;
                                    }
                                }

                                break;
                            }
                        }
                    }

                    this->UpdateSaveDatabaseState();

                    // Now mark the app state as 'complete' so we can roll back to the 'ContinueGame' app state
                    this->completed = true;
                }
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
            {
                if (this->userProfileIndex > 0)
                {
                    this->ClearAllInputs();
                    this->recordedCompetitionStats.pop_back();

                    --this->userProfileIndex;
                }
                else
                    this->exitState = true;
            }
        }

        if (this->selectedCup)
        {
            // Display the 'Did you win the final' radio buttons if the competition is a CUP and the FINAL round was selected
            if (this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() != -1 &&
                this->selectedCup->GetRounds()[this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() - 1] == "Final")
            {
                this->userInterface.GetRadioButtonGroup("Won Cup")->SetOpacity(255.0f);
            }
            else
            {
                this->userInterface.GetRadioButtonGroup("Won Cup")->SetOpacity(0.0f);
            }
        }
        else
        {
            // Display the 'Did you win the playoffs' radio buttons if the competition is a LEAGUE and the user finished in a playoffs spot
            const int userTablePosition = (!this->userInterface.GetTextField("Table Position")->GetInputtedText().empty()) ? 
                std::stoi(this->userInterface.GetTextField("Table Position")->GetInputtedText()) : -1;

            if (userTablePosition > SaveData::GetInstance().GetCurrentLeague()->GetAutoPromotionThreshold() &&
                userTablePosition <= SaveData::GetInstance().GetCurrentLeague()->GetPlayoffsThreshold())
            {
                this->userInterface.GetRadioButtonGroup("Won Playoffs")->SetOpacity(255.0f);
            }
            else
            {
                this->userInterface.GetRadioButtonGroup("Won Playoffs")->SetOpacity(0.0f);
            }
        }
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the fade out effect of the user interface
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));

        if (this->userInterface.GetOpacity() == 0.0f)
        {
            if (this->completed)
                this->SwitchState(MainGame::GetAppState());
            else
            {
                EndCompetition::GetAppState()->GetCompetitionSelectionList().Reset();
                this->PopState();
            }
        }
    }
}

void RecordCompetition::Render() const
{
    // Render the current user, who's entering their stats, name.
    Renderer::GetInstance().RenderShadowedText({ 310, 80 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        SaveData::GetInstance().GetUsers()[this->userProfileIndex].GetName().data() + std::string("'s competition stats"), 5);

    // Render the text descriptions
    Renderer::GetInstance().RenderShadowedText({ 30, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of goals you scored:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of goals you conceded:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of games you won:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 820 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of games you drew:", 5);

    Renderer::GetInstance().RenderShadowedText({ 1200, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of games you lost:", 5);

    if (this->selectedCup)
    {
        Renderer::GetInstance().RenderShadowedText({ 1200, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            "Select the last round you reached", 5);

        if (this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() != -1 && 
            this->selectedCup->GetRounds()[this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() - 1] == "Final")
        {
            Renderer::GetInstance().RenderShadowedText({ 1200, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
                "Did you win the final?", 5);
        }
    }
    else
    {
        Renderer::GetInstance().RenderShadowedText({ 1200, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            "Enter the table position you finished in:", 5);

        const int userTablePosition = (!this->userInterface.GetTextField("Table Position")->GetInputtedText().empty()) ?
            std::stoi(this->userInterface.GetTextField("Table Position")->GetInputtedText()) : -1;

        if (userTablePosition > SaveData::GetInstance().GetCurrentLeague()->GetAutoPromotionThreshold() &&
            userTablePosition <= SaveData::GetInstance().GetCurrentLeague()->GetPlayoffsThreshold())
        {
            Renderer::GetInstance().RenderShadowedText({ 1200, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
                "Did you win the playoffs final?", 5);
        }
    }

    // Render any input validation errors that occur
    if (this->goalsScoredInvalid)
        Renderer::GetInstance().RenderText({ 660, 260 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->goalsConcededInvalid)
        Renderer::GetInstance().RenderText({ 660, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->gamesWonInvalid)
        Renderer::GetInstance().RenderText({ 660, 700 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->gamesDrawnInvalid)
        Renderer::GetInstance().RenderText({ 660, 920 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->gamesLostInvalid)
        Renderer::GetInstance().RenderText({ 1830, 260 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->selectedCup)
    {
        if (this->roundsInvalid)
            Renderer::GetInstance().RenderText({ 1830, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

        if (this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() != -1 &&
            this->selectedCup->GetRounds()[this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() - 1] == "Final")
        {
            if (this->wonCupInvalid)
                Renderer::GetInstance().RenderText({ 1830, 700 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");
        }
    }
    else
    {
        if (this->tablePositionInvalid)
            Renderer::GetInstance().RenderText({ 1830, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

        const int userTablePosition = (!this->userInterface.GetTextField("Table Position")->GetInputtedText().empty()) ?
            std::stoi(this->userInterface.GetTextField("Table Position")->GetInputtedText()) : -1;

        if (userTablePosition > SaveData::GetInstance().GetCurrentLeague()->GetAutoPromotionThreshold() &&
            userTablePosition <= SaveData::GetInstance().GetCurrentLeague()->GetPlayoffsThreshold())
        {
            if (this->wonPlayOffsInvalid)
                Renderer::GetInstance().RenderText({ 1830, 700 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");
        }
    }

    // Render the user interface
    this->userInterface.Render();
}

bool RecordCompetition::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

RecordCompetition* RecordCompetition::GetAppState()
{
    static RecordCompetition appState;
    return &appState;
}
