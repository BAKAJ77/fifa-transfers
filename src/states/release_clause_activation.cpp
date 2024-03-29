#include <states/release_clause_activation.h>
#include <states/contract_negotiation.h>
#include <states/contract_response.h>
#include <states/search_players.h>
#include <states/main_game.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/data_manip.h>

void ReleaseClauseActivation::Init()
{
    // Initialize the member variables
    this->exitState = this->finishedNegotiations = false;
    this->previousClubID = this->targettedPlayer->GetClub();
    this->releaseClauseFee = this->targettedPlayer->GetReleaseClause();

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
    
    if (MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget() >= this->targettedPlayer->GetReleaseClause())
        this->userInterface.AddButton(new MenuButton({ 1745, 880 }, { 300, 100 }, { 315, 115 }, "CONTINUE"));
}

void ReleaseClauseActivation::Destroy()
{
    if (this->finishedNegotiations && ContractResponse::GetAppState()->WasNegotiationsSuccessful())
    {
        Club* currentUserClub = MainGame::GetAppState()->GetCurrentUser()->GetClub();
        Club* sellingClub = SaveData::GetInstance().GetClub(this->previousClubID);

        // Update the transfer budget balances of the buying and selling club
        currentUserClub->SetTransferBudget(currentUserClub->GetTransferBudget() - this->releaseClauseFee);
        sellingClub->SetTransferBudget(sellingClub->GetTransferBudget() + this->releaseClauseFee);

        // Add the transfer data into the transfer history database
        SaveData::GetInstance().GetTransferHistory().push_back({ this->targettedPlayer->GetID(), this->previousClubID, this->targettedPlayer->GetClub(), 
            this->releaseClauseFee });

        // If player was bought from a club that is controlled by another user, then send a general message notifying them of this transfer being completed
        for (const UserProfile& user : SaveData::GetInstance().GetUsers())
        {
            if (user.GetClub()->GetID() == sellingClub->GetID())
            {
                sellingClub->GetGeneralMessages().push_back({ std::string(currentUserClub->GetName()) + " have paid the " +
                    Util::GetFormattedCashString(this->releaseClauseFee) + " release clause for " + this->targettedPlayer->GetName().data() +
                    " and signed him on a " + std::to_string(this->targettedPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) +
                    " year contract." });

                break;
            }
        }
    }
}

void ReleaseClauseActivation::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of other buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "CONTINUE" && button->WasClicked())
            {
                ContractNegotiation::GetAppState()->SetNegotiatingPlayer(this->targettedPlayer, SearchPlayers::GetAppState(), false, &this->finishedNegotiations);
                this->PushState(ContractNegotiation::GetAppState());
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->PopState();
    }
}

void ReleaseClauseActivation::Render() const
{
    // Render the state background
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderShadowedText({ 830, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "RELEASE CLAUSE ACTIVATION", 5);
    
    // Render state text
    if (MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget() >= this->targettedPlayer->GetReleaseClause())
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 250 }, { 0, 255, 0, this->userInterface.GetOpacity() }, this->font, 95,
            "TRANSFER BUDGET: " + Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget()), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 480 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 45,
            "Are you sure you want to activate the release clause in this contract?", 5);
    }
    else
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 250 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 95,
            "TRANSFER BUDGET: " + Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget()), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 475 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            "You don't have enough transfer funds to activate this player's release clause.", 5);
    }

    Renderer::GetInstance().RenderShadowedText({ 60, 370 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 95,
        "RELEASE CLAUSE: " + Util::GetFormattedCashString(this->targettedPlayer->GetReleaseClause()), 5);

    // Render the user interface
    this->userInterface.Render();
}

bool ReleaseClauseActivation::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool ReleaseClauseActivation::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool ReleaseClauseActivation::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

ReleaseClauseActivation* ReleaseClauseActivation::GetAppState()
{
    static ReleaseClauseActivation appState;
    return &appState;
}

void ReleaseClauseActivation::SetTargettedPlayer(Player* player)
{
    this->targettedPlayer = player;
}
