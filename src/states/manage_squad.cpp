#include <states/manage_squad.h>
#include <states/main_game.h>
#include <states/view_player.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>

void ManageSquad::Init()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(false);

    // Initialize the member variables
    this->exitState = false;
    this->currentUserClub = MainGame::GetAppState()->GetCurrentUser()->GetClub();

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddSelectionList("Players", { { 960, 490 }, { 1860, 720 }, 80, 255, 25 });
    this->userInterface.GetSelectionList("Players")->AddCategory("Name");
    this->userInterface.GetSelectionList("Players")->AddCategory("Nation");
    this->userInterface.GetSelectionList("Players")->AddCategory("Age");
    this->userInterface.GetSelectionList("Players")->AddCategory("Position");
    this->userInterface.GetSelectionList("Players")->AddCategory("Expiry Year");
    
    for (size_t index = 0; index < this->currentUserClub->GetPlayers().size(); index++)
    {
        const Player* player = this->currentUserClub->GetPlayers()[index];

        if (player->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear() == 1)
        {
            // Set the selection element color as YELLOW if the player only has 1 year left on his contract
            this->userInterface.GetSelectionList("Players")->AddElement({ player->GetName().data(), player->GetNation().data(), 
                std::to_string(player->GetAge()), SaveData::GetInstance().GetPosition(player->GetPosition())->type, 
                std::to_string(player->GetExpiryYear()) }, (int)index, { 85, 85, 0 }, { 115, 115, 0 }, { 60, 60, 0 });
        }
        else
        {
            this->userInterface.GetSelectionList("Players")->AddElement({ player->GetName().data(), player->GetNation().data(),
                std::to_string(player->GetAge()), SaveData::GetInstance().GetPosition(player->GetPosition())->type, 
                std::to_string(player->GetExpiryYear()) }, (int)index);
        }
    }
}

void ManageSquad::Destroy()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(true);
}

void ManageSquad::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of othe buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        // Check if a player has been selected from the selection list
        if (this->userInterface.GetSelectionList("Players")->GetCurrentSelected() != -1)
        {
            Player* player = this->currentUserClub->GetPlayers()[this->userInterface.GetSelectionList("Players")->GetCurrentSelected()];
            ViewPlayer::GetAppState()->SetPlayerToView(player);
            this->PushState(ViewPlayer::GetAppState());
        }
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->PopState();
    }
}

void ManageSquad::Render() const
{
    // Render the text description
    Renderer::GetInstance().RenderShadowedText({ 1450, 95 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "CLUB SQUAD", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool ManageSquad::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool ManageSquad::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool ManageSquad::OnResumeTransitionUpdate(const float deltaTime)
{
    if (this->userInterface.GetSelectionList("Players")->GetCurrentSelected() != -1)
        this->userInterface.GetSelectionList("Players")->Reset();

    return this->OnStartupTransitionUpdate(deltaTime);
}

ManageSquad* ManageSquad::GetAppState()
{
    static ManageSquad appState;
    return &appState;
}
