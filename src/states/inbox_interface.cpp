#include <states/inbox_interface.h>
#include <states/transfer_negotiation.h>
#include <states/contract_negotiation.h>
#include <states/contract_response.h>
#include <states/main_game.h>

#include <serialization/save_data.h>
#include <interface/menu_button.h>
#include <util/timestamp.h>
#include <util/data_manip.h>

void InboxInterface::Init()
{
    // Initialize member variables
    this->exitState = this->insufficientTransferFunds = false;
    this->selectedAgreedTransfer = { 0, nullptr, false };

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
    
    if (this->type == InboxType::GENERAL)
        this->userInterface.AddButton(new MenuButton({ 1410, 1005 }, { 300, 100 }, { 315, 115 }, "CLEAR"));

    this->userInterface.AddSelectionList("Inbox Messages", { { 960, 490 }, { 1860, 720 }, 80, 255, 25 });
    this->userInterface.GetSelectionList("Inbox Messages")->AddCategory("Inbox Messages");

    // Load the general/transfer messages
    if (this->type == InboxType::GENERAL)
        this->LoadGeneralMessages();
    else if (this->type == InboxType::TRANSFERS)
        this->LoadTransferMessages();
}

void InboxInterface::Destroy() {}

void InboxInterface::LoadGeneralMessages()
{
    this->userInterface.GetSelectionList("Inbox Messages")->Clear();

    for (const std::string& generalMsg : MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetGeneralMessages())
        this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ generalMsg }, -1);
}

void InboxInterface::LoadTransferMessages()
{
    this->userInterface.GetSelectionList("Inbox Messages")->Clear();

    for (size_t index = 0; index < MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferMessages().size(); index++)
    {
        const Club::Transfer& transferMsg = MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferMessages()[index];

        // The transfer message recieved is a opening/counter offer or a "pulling out of negotiations" message from another buying club 
        if (MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID() != transferMsg.biddingClubID)
        {
            // Fetch the bidding club and the player being bidded for
            Club* biddingClub = SaveData::GetInstance().GetClub(transferMsg.biddingClubID);
            Player* targettedPlayer = SaveData::GetInstance().GetPlayer(transferMsg.playerID);

            if (transferMsg.rejectedOffer)
            {
                this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ std::string(biddingClub->GetName()) +
                     " have decided to pull out of negotiations for " + targettedPlayer->GetName().data() }, -1);
            }
            else
            {
                if (transferMsg.counterOffer) // The transfer message is a counter offer
                {
                    this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ std::string(biddingClub->GetName()) +
                         " have submitted a counter offer of " + Util::GetFormattedCashString(transferMsg.transferFee) + " for " +
                         targettedPlayer->GetName().data() }, (int)index);
                }
                else // The transfer message is a opening offer
                {
                    this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ std::string(biddingClub->GetName()) +
                        " have submitted an opening offer of " + Util::GetFormattedCashString(transferMsg.transferFee) + " for " +
                        targettedPlayer->GetName().data() }, (int)index);
                }
            }
        }
        else // The transfer message recieved is a response message from the selling club
        {
            // Fetch the selling club and the player being bidded for
            Player* targettedPlayer = SaveData::GetInstance().GetPlayer(transferMsg.playerID);
            Club* sellingClub = SaveData::GetInstance().GetClub(targettedPlayer->GetClub());

            if (transferMsg.rejectedOffer) // The transfer message indicates that the selling club out-right rejected the offer
            {
                this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ std::string(sellingClub->GetName()) +
                    " have rejected your approach for " + targettedPlayer->GetName().data() + " and aren't willing to negotiate any further." }, -1);
            }
            else
            {
                if (transferMsg.feeAgreed) // The transfer message indicates that the selling club agreed to the offer the user submitted
                {
                    this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ "You and " + std::string(sellingClub->GetName()) +
                        " have agreed a fee of " + Util::GetFormattedCashString(transferMsg.transferFee) + " for " + targettedPlayer->GetName().data() +
                        ", you can now negotiate a contract with the player." }, (int)index);
                }
                else // The transfer message indicates the selling club asking for a better transfer fee for the player being bidded for
                {
                    this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ std::string(sellingClub->GetName()) +
                        " believe a transfer fee in the region of " + Util::GetFormattedCashString(transferMsg.transferFee) +
                        " would be more suitable for " + targettedPlayer->GetName().data() + "." }, (int)index);
                }
            }
        }
    }
}

void InboxInterface::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];

            if (button->GetText() == "CLEAR" && button->WasClicked())
            {
                MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetGeneralMessages().clear();
                this->userInterface.GetSelectionList("Inbox Messages")->Clear();
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        // Check if any of the active transfer messages in the inbox have been clicked
        const int selectedTransferMsgIndex = this->userInterface.GetSelectionList("Inbox Messages")->GetCurrentSelected();

        if (selectedTransferMsgIndex != -1)
        {
            const Club::Transfer& transferMsg = MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferMessages()[selectedTransferMsgIndex];

            if (MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID() != transferMsg.biddingClubID)
            {
                TransferNegotiation::GetAppState()->SetTargettedPlayer(SaveData::GetInstance().GetPlayer(transferMsg.playerID), &transferMsg);
                this->PushState(TransferNegotiation::GetAppState());
            }
            else
            {
                if (transferMsg.feeAgreed)
                {
                    if (MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget() >= transferMsg.transferFee)
                    {
                        this->selectedAgreedTransfer.sellingClubID = SaveData::GetInstance().GetPlayer(transferMsg.playerID)->GetClub();
                        this->selectedAgreedTransfer.transferMsg = &transferMsg;

                        ContractNegotiation::GetAppState()->SetNegotiatingPlayer(SaveData::GetInstance().GetPlayer(transferMsg.playerID), this, false,
                            &this->selectedAgreedTransfer.finishedNegotiating);

                        this->PushState(ContractNegotiation::GetAppState());
                    }
                    else
                        this->insufficientTransferFunds = true;
                }
                else
                {
                    TransferNegotiation::GetAppState()->SetTargettedPlayer(SaveData::GetInstance().GetPlayer(transferMsg.playerID), &transferMsg);
                    this->PushState(TransferNegotiation::GetAppState());
                }
            }

            this->userInterface.GetSelectionList("Inbox Messages")->Reset();
        }

        // If the user has agreed a transfer fee with another club, check if they've finished negotiating the contract with the player
        if (this->selectedAgreedTransfer.transferMsg)
        {
            if (ContractNegotiation::GetAppState()->WentBack())
            {
                this->selectedAgreedTransfer = { 0, nullptr, false };
            }
            else if (this->selectedAgreedTransfer.finishedNegotiating)
            {
                Player* transferredPlayer = SaveData::GetInstance().GetPlayer(this->selectedAgreedTransfer.transferMsg->playerID);
                Club* sellerClub = SaveData::GetInstance().GetClub(this->selectedAgreedTransfer.sellingClubID);
                Club* currentUserClub = MainGame::GetAppState()->GetCurrentUser()->GetClub();

                if (ContractResponse::GetAppState()->WasNegotiationsSuccessful())
                {
                    // Update the transfer budget balances of both clubs involved
                    sellerClub->SetTransferBudget(sellerClub->GetTransferBudget() + this->selectedAgreedTransfer.transferMsg->transferFee);
                    currentUserClub->SetTransferBudget(currentUserClub->GetTransferBudget() - this->selectedAgreedTransfer.transferMsg->transferFee);

                    // Send general message to the seller club that the transfer has been completed (if the seller club is controlled by a user)
                    for (const UserProfile& user : SaveData::GetInstance().GetUsers())
                    {
                        if (user.GetClub()->GetID() == sellerClub->GetID())
                        {
                            sellerClub->GetGeneralMessages().push_back(std::string(currentUserClub->GetName()) + " have successfully signed " +
                                transferredPlayer->GetName().data() + " on a " +
                                std::to_string(transferredPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) + 
                                " year contract for a transfer fee of " + Util::GetFormattedCashString(this->selectedAgreedTransfer.transferMsg->transferFee) + ".");

                            break;
                        }
                    }

                    // Add the transfer into the transfer history database
                    SaveData::GetInstance().GetTransferHistory().push_back({ this->selectedAgreedTransfer.transferMsg->playerID,
                        this->selectedAgreedTransfer.sellingClubID, this->selectedAgreedTransfer.transferMsg->biddingClubID,
                        this->selectedAgreedTransfer.transferMsg->transferFee });
                }
                else
                {
                    // Send general message to the seller club that the transfer has broken down (if the seller club is controlled by a user)
                    for (const UserProfile& user : SaveData::GetInstance().GetUsers())
                    {
                        if (user.GetClub()->GetID() == sellerClub->GetID())
                        {
                            sellerClub->GetGeneralMessages().push_back("Contract negotiations between " + std::string(currentUserClub->GetName()) + " and " +
                                transferredPlayer->GetName().data() + " have broken down, therefore " + transferredPlayer->GetName().data() +
                                " will remain at your club.");

                            break;
                        }
                    }
                }
                

                // Remove the transfer message from the buyer user
                for (size_t index = 0; index < currentUserClub->GetTransferMessages().size(); index++)
                {
                    if (this->selectedAgreedTransfer.transferMsg == &currentUserClub->GetTransferMessages()[index])
                    {
                        currentUserClub->GetTransferMessages().erase(currentUserClub->GetTransferMessages().begin() + index);
                        break;
                    }
                }

                // Reset the current inbox state
                this->selectedAgreedTransfer.sellingClubID = 0;
                this->selectedAgreedTransfer.transferMsg = nullptr;
                this->selectedAgreedTransfer.finishedNegotiating = false;

                this->LoadTransferMessages();
            }
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

void InboxInterface::Render() const
{
    // Render the app state title
    if (this->type == InboxType::GENERAL)
    {
        Renderer::GetInstance().RenderShadowedText({ 1320, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
            "GENERAL INBOX", 5);
    }
    else
    {
        Renderer::GetInstance().RenderShadowedText({ 1280, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
            "TRANSFER INBOX", 5);
    }

    // Render error text if the user tries to complete a transfer and has insufficent transfer funds 
    this->RenderInsufficientTransferFundsText();

    // Render the user interface
    this->userInterface.Render();
}

void InboxInterface::RenderInsufficientTransferFundsText() const
{
    if (this->insufficientTransferFunds)
    {
        Renderer::GetInstance().RenderShadowedText({ 30, 1050 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 40,
            "You don't have enough transfer funds to complete this transfer.", 5);

        static bool capturedPreviousTime = false;
        static float previousTime = 0.0f;

        if (!capturedPreviousTime)
        {
            previousTime = Util::GetSecondsSinceEpoch();
            capturedPreviousTime = true;
        }

        if (Util::GetSecondsSinceEpoch() - previousTime >= 3.0f)
        {
            this->insufficientTransferFunds = false;
            capturedPreviousTime = false;
        }
    }
}

bool InboxInterface::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool InboxInterface::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool InboxInterface::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

InboxInterface* InboxInterface::GetAppState()
{
    static InboxInterface appState;
    return &appState;
}

void InboxInterface::SetInboxType(InboxType type)
{
    this->type = type;
}
