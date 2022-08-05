#include <states/transfer_negotiation.h>
#include <states/inbox_interface.h>
#include <states/main_game.h>

#include <serialization/save_data.h>
#include <interface/menu_button.h>
#include <util/random_engine.h>
#include <util/data_manip.h>

void TransferNegotiation::Init()
{
    // Initialize member variables
    this->exitState = this->onNegotiationCooldown = this->playerNotForSale = this->actionInvalid = this->bidAmountInvalid = this->alreadyNegotiating = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    if (!this->existingTransferNegotiation)
    {
        // Make sure that the user is not already currently negotiating with the selling club for the player
        for (const Club::Transfer& transferMsg : SaveData::GetInstance().GetClub(this->targettedPlayer->GetClub())->GetTransferMessages())
        {
            if (transferMsg.biddingClubID == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID() &&
                transferMsg.playerID == this->targettedPlayer->GetID() && !transferMsg.rejectedOffer)
            {
                this->alreadyNegotiating = true;
                break;
            }
        }

        for (const Club::Transfer& transferMsg : MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferMessages())
        {
            if (transferMsg.playerID == this->targettedPlayer->GetID() && !transferMsg.rejectedOffer)
            {
                this->alreadyNegotiating = true;
                break;
            }
        }

        // Check if there is an active negotiation cooldown between the user's club and the other club for the player
        for (const SaveData::NegotiationCooldown& cooldown : SaveData::GetInstance().GetNegotiationCooldowns())
        {
            if ((cooldown.clubID == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID() || cooldown.clubID == 0) &&
                cooldown.playerID == this->targettedPlayer->GetID() && cooldown.type == SaveData::CooldownType::TRANSFER_NEGOTIATING)
            {
                this->onNegotiationCooldown = true;
                break;
            }
        }

        // If the player has at least 4 years on his contract and is in the top 3 players at the club, then state that the player is not for sale,
        // also if the player has 3 years left on his contract, then allow a chance that the club will be willing to let him go.
        // 
        // This step is only done when the selling club is not controlled by a user.
        bool sellingClubBelongsToUser = false;

        for (const UserProfile& user : SaveData::GetInstance().GetUsers())
        {
            if (user.GetClub()->GetID() == this->targettedPlayer->GetClub())
            {
                sellingClubBelongsToUser = true;
                break;
            }
        }

        if (!sellingClubBelongsToUser)
        {
            int totalBetterPlayers = 0;
            for (const Player* player : SaveData::GetInstance().GetClub(this->targettedPlayer->GetClub())->GetPlayers())
            {
                if (player->GetOverall() > this->targettedPlayer->GetOverall())
                    ++totalBetterPlayers;
            }

            if (totalBetterPlayers < 3)
            {
                const int yearsLeftOnContract = this->targettedPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear();

                if (yearsLeftOnContract > 3)
                    this->playerNotForSale = true;
                else if (yearsLeftOnContract == 3)
                {
                    const int generatedWeight = RandomEngine::GetInstance().GenerateRandom<int>(0, 100);
                    if (generatedWeight < 30)
                        this->playerNotForSale = true;
                }
            }
        }
    }

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    if (!this->onNegotiationCooldown && !this->playerNotForSale && !this->alreadyNegotiating)
    {
        if (this->existingTransferNegotiation)
        {
            this->userInterface.AddRadioButtonGroup("Action", RadioButtonGroup({ 80, 805 }, { 40, 40 }));
            this->userInterface.GetRadioButtonGroup("Action")->Add("Accept", 0);
            this->userInterface.GetRadioButtonGroup("Action")->Add("Reject", 1);
            this->userInterface.GetRadioButtonGroup("Action")->Add("Counter", 2);

            this->userInterface.AddTextField("Transfer Fee", TextInputField({ 210, 975 }, { 300, 75 }, TextInputField::Restrictions::NO_ALPHABETIC |
                TextInputField::Restrictions::NO_SPACES, 0.0f));
        }
        else
        {
            this->userInterface.AddTextField("Transfer Fee", TextInputField({ 210, 830 }, { 300, 75 }, TextInputField::Restrictions::NO_ALPHABETIC |
                TextInputField::Restrictions::NO_SPACES));
        }

        this->userInterface.AddButton(new MenuButton({ 1745, 880 }, { 300, 100 }, { 315, 115 }, "SUBMIT"));
    }
}

void TransferNegotiation::Destroy() {}

bool TransferNegotiation::ValidateInput()
{
    // Make sure an action has been selected if this isn't an opening offer)
    if (this->existingTransferNegotiation)
        this->actionInvalid = (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == -1);

    // Make sure an amount has been entered into the text box if this is an opening offer or if the 'Counter' action has been selected
    this->bidAmountInvalid = this->userInterface.GetTextField("Transfer Fee")->GetInputtedText().empty();
    if (this->existingTransferNegotiation)
        this->bidAmountInvalid = (this->bidAmountInvalid && this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 2);

    // Make sure the entered amount, if the user is bidding, is within the user's transfer budget
    if (!this->userInterface.GetTextField("Transfer Fee")->GetInputtedText().empty() &&
        this->targettedPlayer->GetClub() != MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID())
    {
        this->bidAmountInvalid = (std::stoi(this->userInterface.GetTextField("Transfer Fee")->GetInputtedText()) >
            MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget());
    }

    return !this->actionInvalid && !this->bidAmountInvalid;
}

void TransferNegotiation::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "SUBMIT" && button->WasClicked() && this->ValidateInput())
            {
                Club* const currentUserClub = MainGame::GetAppState()->GetCurrentUser()->GetClub();
                Club* const sellerClub = SaveData::GetInstance().GetClub(this->targettedPlayer->GetClub());
                
                if (this->existingTransferNegotiation)
                {
                    Club::Transfer transferMsg;
                    transferMsg.biddingClubID = this->existingTransferNegotiation->biddingClubID;
                    transferMsg.playerID = this->existingTransferNegotiation->playerID;
                    transferMsg.transferFee = this->existingTransferNegotiation->transferFee;
                    transferMsg.expirationTicks = 3;

                    if (this->existingTransferNegotiation->biddingClubID == currentUserClub->GetID()) // The current user is the buyer
                    {
                        if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 0) // The buyer user has accepted the seller's wanted transfer fee
                        {
                            transferMsg.feeAgreed = true;

                            // Remove the given transfer message from the user club's transfer inbox
                            // We do this here because the 'existingTransferNegotiation' pointer will become invalid after the emplace_back() call
                            // due to reallocation of the vector.
                            for (size_t index = 0; index < currentUserClub->GetTransferMessages().size(); index++)
                            {
                                if (this->existingTransferNegotiation == &currentUserClub->GetTransferMessages()[index])
                                {
                                    currentUserClub->GetTransferMessages().erase(currentUserClub->GetTransferMessages().begin() + index);
                                    break;
                                }
                            }
                            
                            // We send this message to the buyer user instead of the seller so ther user can continue onto the contract negotiations from their inbox
                            currentUserClub->GetTransferMessages().emplace_back(transferMsg);
                        }
                        else if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 1) // The buyer user has rejected the seller's wanted transfer fee
                        {
                            transferMsg.rejectedOffer = true;
                            sellerClub->GetTransferMessages().emplace_back(transferMsg);
                        }
                        else if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 2) // The buyer has submitted a new counter bid
                        {
                            transferMsg.counterOffer = true;
                            transferMsg.transferFee = std::stoi(this->userInterface.GetTextField("Transfer Fee")->GetInputtedText());
                            sellerClub->GetTransferMessages().emplace_back(transferMsg);
                        }
                    }
                    else // The current user is the seller
                    {
                        Club* biddingClub = SaveData::GetInstance().GetClub(this->existingTransferNegotiation->biddingClubID);

                        if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 0) // The seller user accepted the bid from the buyer
                        {
                            transferMsg.feeAgreed = true;
                            biddingClub->GetTransferMessages().emplace_back(transferMsg);
                        }
                        else if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 1) // The seller user out-right rejected the bid
                        {
                            transferMsg.rejectedOffer = true;
                            biddingClub->GetTransferMessages().emplace_back(transferMsg);
                        }
                        else if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 2) // The seller user sent the user the transfer fee they want
                        {
                            transferMsg.transferFee = std::stoi(this->userInterface.GetTextField("Transfer Fee")->GetInputtedText());
                            biddingClub->GetTransferMessages().emplace_back(transferMsg);
                        }
                    }

                    if (!(this->existingTransferNegotiation->biddingClubID == currentUserClub->GetID() &&
                        this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 0))
                    {
                        // Remove the given transfer message from the user club's transfer inbox
                        for (size_t index = 0; index < currentUserClub->GetTransferMessages().size(); index++)
                        {
                            if (this->existingTransferNegotiation == &currentUserClub->GetTransferMessages()[index])
                            {
                                currentUserClub->GetTransferMessages().erase(currentUserClub->GetTransferMessages().begin() + index);
                                break;
                            }
                        }
                    }

                    InboxInterface::GetAppState()->LoadTransferMessages();
                }
                else
                {
                    // Send the opening transfer bid to the selling club's transfer inbox
                    Club::Transfer transferBid;
                    transferBid.biddingClubID = currentUserClub->GetID();
                    transferBid.playerID = this->targettedPlayer->GetID();
                    transferBid.transferFee = std::stoi(this->userInterface.GetTextField("Transfer Fee")->GetInputtedText());
                    transferBid.expirationTicks = 3;
                    
                    sellerClub->GetTransferMessages().emplace_back(transferBid);
                }

                this->exitState = true;
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        // Show the counter bid option if the 'Counter' radio button was selected, else hide it
        if (this->existingTransferNegotiation)
        {
            if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 2)
                this->userInterface.GetTextField("Transfer Fee")->SetOpacity(255);
            else
                this->userInterface.GetTextField("Transfer Fee")->SetOpacity(0);
        }
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the fade out effect of the user interface
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        if (this->userInterface.GetOpacity() == 0.0f)
            this->PopState();
    }
}

void TransferNegotiation::Render() const
{
    // Render the stats background
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderShadowedText({ 1020, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "TRANSFER NEGOTIATION", 5);

    if (!this->onNegotiationCooldown && !this->playerNotForSale && !this->alreadyNegotiating)
    {
        // Render the relevant player stats text
        Renderer::GetInstance().RenderShadowedText({ 60, 215 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 55,
            std::string("NAME: ") + this->targettedPlayer->GetName().data(), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 290 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 55,
            "AGE: " + std::to_string(this->targettedPlayer->GetAge()), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 365 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 55,
            "POSITION: " + SaveData::GetInstance().GetPosition(this->targettedPlayer->GetPosition())->type, 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 440 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 55,
            "VALUE: " + Util::GetFormattedCashString(this->targettedPlayer->GetValue()), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 515 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 55,
            "WAGE: " + Util::GetFormattedCashString(this->targettedPlayer->GetWage()), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 590 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 55,
            "CONTRACT EXPIRY: " + std::to_string(this->targettedPlayer->GetExpiryYear()) +
            " (" + std::to_string(this->targettedPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) + " YEARS LEFT)", 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 665 }, { 0, 200, 200, this->userInterface.GetOpacity() }, this->font, 55,
            "TRANSFER BUDGET: " + Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget()), 5);

        if (this->existingTransferNegotiation)
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 755 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
                "Select the action you want to proceed with", 5);

            if (this->userInterface.GetRadioButtonGroup("Action")->GetSelected() == 2)
            {
                if (this->existingTransferNegotiation->biddingClubID == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID())
                {
                    Renderer::GetInstance().RenderShadowedText({ 60, 900 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
                        "Enter the amount you want to counter bid for this player:", 5);
                }
                else
                {
                    Renderer::GetInstance().RenderShadowedText({ 60, 900 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
                        "Enter the transfer fee you want for the player:", 5);
                }
            }
        }
        else
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 755 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
                "Enter the amount you want to bid for this player:", 5);
        }
    }
    else if (this->alreadyNegotiating)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 27, 
            "You're already currently in negotiation talks with " + std::string(SaveData::GetInstance().GetClub(this->targettedPlayer->GetClub())->GetName()) + 
            " for this player.", 5);
    }
    else if (this->playerNotForSale)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 27,
            std::string(SaveData::GetInstance().GetClub(this->targettedPlayer->GetClub())->GetName()) +
            " aren't willing to sell this player at the moment since he's one of their best players.", 5);
    }
    else if (this->onNegotiationCooldown)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
            std::string(SaveData::GetInstance().GetClub(this->targettedPlayer->GetClub())->GetName()) + 
            " aren't interested in negotiating with you for this player at the moment.", 5);
    }

    // Render any input validation errors that occur
    if (this->actionInvalid)
        Renderer::GetInstance().RenderText({ 730, 820 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->bidAmountInvalid)
    {
        if (this->existingTransferNegotiation)
        {
            Renderer::GetInstance().RenderText({ 390, 990 },
                { 255, 0, 0, (this->userInterface.GetOpacity() * this->userInterface.GetTextField("Transfer Fee")->GetOpacity()) / 255 }, this->font, 30, "*");
        }
        else
        {
            Renderer::GetInstance().RenderText({ 390, 845 },
                { 255, 0, 0, (this->userInterface.GetOpacity() * this->userInterface.GetTextField("Transfer Fee")->GetOpacity()) / 255 }, this->font, 30, "*");
        }
    }

    // Render the user interface
    this->userInterface.Render();
}

bool TransferNegotiation::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

TransferNegotiation* TransferNegotiation::GetAppState()
{
    static TransferNegotiation appState;
    return &appState;
}

void TransferNegotiation::SetTargettedPlayer(Player* player, const Club::Transfer* existingTransferNegotiation)
{
    this->targettedPlayer = player;
    this->existingTransferNegotiation = existingTransferNegotiation;
}
