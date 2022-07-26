#include <states/view_player.h>
#include <states/main_game.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/data_manip.h>

void ViewPlayer::Init()
{
    // Initialize the member variables
    this->exitState = false;
    this->usingOverallGrowth = (SaveData::GetInstance().GetGrowthSystemType() == SaveData::GrowthSystemType::OVERALL_RATING);

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    if (this->displayedPlayer->GetClub() == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID())
    {
        this->userInterface.AddTickBox("Block Transfers", TickBox({ 60, 750 + ((int)usingOverallGrowth * 60) }, glm::vec2(40), "BLOCK TRANSFERS?", 255, 5,
            this->displayedPlayer->GetTransfersBlocked()));

        // Don't display the transfer list option if transfers have been blocked for the player
        this->userInterface.AddTickBox("Transfer List", TickBox({ 60, 810 + ((int)usingOverallGrowth * 60) }, glm::vec2(40), "TRANSFER LIST?",
            this->displayedPlayer->GetTransfersBlocked() ? 0.0f : 255.0f, 10, this->displayedPlayer->GetTransferListed()));

        this->userInterface.AddButton(new MenuButton({ 410, 910 + ((int)usingOverallGrowth * 60) }, { 700, 100 }, { 700, 100 }, "RENEW CONTRACT"));
    }
    else
    {
        this->userInterface.AddButton(new MenuButton({ 410, 780 + ((int)usingOverallGrowth * 60) }, { 700, 100 }, { 700, 100 }, 
            "INITIATE TRANSFER TALKS"));

        if (this->displayedPlayer->GetReleaseClause() > 0)
            this->userInterface.AddButton(new MenuButton({ 410, 910 + ((int)usingOverallGrowth * 60) }, { 700, 100 }, { 700, 100 }, 
                "ACTIVATE RELEASE CLAUSE"));
    }
}

void ViewPlayer::Destroy() 
{
    if (this->displayedPlayer->GetClub() == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID())
    {
        this->displayedPlayer->SetTransfersBlocked(this->userInterface.GetTickBox("Block Transfers")->isCurrentlyTicked());
        this->displayedPlayer->SetTransferListed(this->userInterface.GetTickBox("Transfer List")->isCurrentlyTicked());
    }
}

void ViewPlayer::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of othe buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];

            if (button->GetText() == "RENEW CONTRACT" && button->WasClicked())
            {
                // TODO: Implement contract renewal system
            }
            else if (button->GetText() == "INITIATE TRANSFER TALKS" && button->WasClicked())
            {
                // TODO: Implement transfer initiation system
            }
            else if (button->GetText() == "ACTIVATE RELEASE CLAUSE" && button->WasClicked())
            {
                // TODO: Implement release clause activation system
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        if (this->displayedPlayer->GetClub() == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID())
        {
            // Hide and reset the transfer list option if the block transfers option is enabled
            if (this->userInterface.GetTickBox("Block Transfers")->isCurrentlyTicked())
            {
                this->userInterface.GetTickBox("Transfer List")->SetOpacity(0);
                this->userInterface.GetTickBox("Transfer List")->Reset();
            }
            else // Show it if the block transfers option is disabled
                this->userInterface.GetTickBox("Transfer List")->SetOpacity(255);
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

void ViewPlayer::Render() const
{
    // Render the stats background
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderShadowedText({ 1420, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "PLAYER VIEW", 5);

    // Render the player stats text
    Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        std::string("NAME: ") + this->displayedPlayer->GetName().data(), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 260 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "AGE: " + std::to_string(this->displayedPlayer->GetAge()), 5);

    if (this->displayedPlayer->GetClub() == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID())
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 320 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            std::string("NATIONALITY: ") + this->displayedPlayer->GetNation().data(), 5);
    }
    else
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 320 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            std::string("CLUB: ") + SaveData::GetInstance().GetClub(this->displayedPlayer->GetClub())->GetName().data() + " (NATIONALITY: " +
            this->displayedPlayer->GetNation().data() + ")", 5);
    }

    if (SaveData::GetInstance().GetGrowthSystemType() == SaveData::GrowthSystemType::OVERALL_RATING)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 380 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            "OVERALL: " + std::to_string(this->displayedPlayer->GetOverall()), 5);
    }

    Renderer::GetInstance().RenderShadowedText({ 60, 380 + ((int)usingOverallGrowth * 60) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
        this->font, 40, "POSITION: " + SaveData::GetInstance().GetPosition(this->displayedPlayer->GetPosition())->type, 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 440 + ((int)usingOverallGrowth * 60) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
        this->font, 40, std::string("PREFERRED FOOT: ") + this->displayedPlayer->GetPreferredFoot().data(), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 520 + ((int)usingOverallGrowth * 60) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
        this->font, 40, "VALUE: " + Util::GetFormattedCashString(this->displayedPlayer->GetValue()), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 580 + ((int)usingOverallGrowth * 60) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
        this->font, 40, "WAGE: " + Util::GetFormattedCashString(this->displayedPlayer->GetWage()), 5);

    if (this->displayedPlayer->GetReleaseClause() > 0)
        Renderer::GetInstance().RenderShadowedText({ 60, 640 + ((int)usingOverallGrowth * 60) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
            this->font, 40, "RELEASE CLAUSE: " + Util::GetFormattedCashString(this->displayedPlayer->GetReleaseClause()), 5);
    else
        Renderer::GetInstance().RenderShadowedText({ 60, 640 + ((int)usingOverallGrowth * 60) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
            this->font, 40, "RELEASE CLAUSE: N/A", 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 700 + ((int)usingOverallGrowth * 60) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
        this->font, 40, "CONTRACT EXPIRY: " + std::to_string(this->displayedPlayer->GetExpiryYear()) + 
        " (" + std::to_string(this->displayedPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) + " YEARS LEFT)", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool ViewPlayer::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

ViewPlayer* ViewPlayer::GetAppState()
{
    static ViewPlayer appState;
    return &appState;
}

void ViewPlayer::SetPlayerToView(Player* player)
{
    this->displayedPlayer = player;
}
