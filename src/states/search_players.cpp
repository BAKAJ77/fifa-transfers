#include <states/search_players.h>
#include <states/main_game.h>
#include <states/view_player.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>

void SearchPlayers::Init()
{
    // Initialize the member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddTextField("Name", TextInputField({ 400, 165 }, { 400, 70 }, TextInputField::Restrictions::NO_NUMERIC, 255, 2.5f));
    this->userInterface.AddTextField("Club", TextInputField({ 1000, 165 }, { 400, 70 }, TextInputField::Restrictions::NONE, 255, 2.5f));
    this->userInterface.AddTextField("Position", TextInputField({ 1670, 165 }, { 400, 70 }, TextInputField::Restrictions::NO_SPACES, 255, 2.5f));

    this->userInterface.AddSelectionList("Players", { { 960, 550 }, { 1860, 620 }, 80, 255, 25 });
    this->userInterface.GetSelectionList("Players")->AddCategory("Name");
    this->userInterface.GetSelectionList("Players")->AddCategory("Club");
    this->userInterface.GetSelectionList("Players")->AddCategory("Position");
}

void SearchPlayers::Destroy() {}

void SearchPlayers::UpdateSelectionList()
{
    // Tranform the filter inputs into upper case
    std::string playerNameFilter = this->userInterface.GetTextField("Name")->GetInputtedText();
    std::string clubNameFilter = this->userInterface.GetTextField("Club")->GetInputtedText();
    std::string positionFilter = this->userInterface.GetTextField("Position")->GetInputtedText();

    std::transform(playerNameFilter.begin(), playerNameFilter.end(), playerNameFilter.begin(), ::toupper);
    std::transform(clubNameFilter.begin(), clubNameFilter.end(), clubNameFilter.begin(), ::toupper);
    std::transform(positionFilter.begin(), positionFilter.end(), positionFilter.begin(), ::toupper);

    if (playerNameFilter.empty() && clubNameFilter.empty() && positionFilter.empty() && 
        !this->userInterface.GetSelectionList("Players")->GetListElements().empty())
    {
        this->userInterface.GetSelectionList("Players")->Clear();
    }
    else if (this->previousNameEntry != playerNameFilter || this->previousClubEntry != clubNameFilter || this->previousPositionEntry != positionFilter)
    {
        // Update the selection list with players matching the filter settings specified
        this->userInterface.GetSelectionList("Players")->Clear();

        for (size_t playerIndex = 0; playerIndex < SaveData::GetInstance().GetPlayerDatabase().size(); playerIndex++)
        {
            // Retrieve the player, club and position from the save database
            const Player& player = SaveData::GetInstance().GetPlayerDatabase()[playerIndex];
            const Club& club = *SaveData::GetInstance().GetClub(player.GetClub());
            const SaveData::Position& position = *SaveData::GetInstance().GetPosition(player.GetPosition());

            // Add the players which match the filters specified into the selection list and aren't already in the current user's club
            if ((playerNameFilter.empty() || player.GetName().find(playerNameFilter) != std::string_view::npos) &&
                (clubNameFilter.empty() || club.GetName().find(clubNameFilter) != std::string_view::npos) &&
                (positionFilter.empty() || position.type.find(positionFilter) != std::string::npos) && 
                club.GetID() != MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID())
            {
                this->userInterface.GetSelectionList("Players")->AddElement({ player.GetName().data(), club.GetName().data(), position.type }, 
                    (int)playerIndex);
            }
        }

        // This to keep track so we don't need to update if no filters have been modified
        this->previousNameEntry = playerNameFilter;
        this->previousClubEntry = clubNameFilter;
        this->previousPositionEntry = positionFilter;
    }
}

void SearchPlayers::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of other buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        // Check if a player has been selected in the selection list
        const int playerSelectionIndex = this->userInterface.GetSelectionList("Players")->GetCurrentSelected();
        if (playerSelectionIndex != -1)
        {
            this->userInterface.GetSelectionList("Players")->Reset();

            ViewPlayer::GetAppState()->SetPlayerToView(&SaveData::GetInstance().GetPlayerDatabase()[playerSelectionIndex]);
            this->PushState(ViewPlayer::GetAppState());
        }
        
        // Update the selection list options
        this->UpdateSelectionList();
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->PopState();
    }
}

void SearchPlayers::Render() const
{
    // Render the filter background bar
    Renderer::GetInstance().RenderSquare({ 960, 165 }, { 1860, 110 }, { glm::vec3(30), this->userInterface.GetOpacity() });

    // Render the filter label texts
    Renderer::GetInstance().RenderShadowedText({ 50, 180 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40, "Name: ", 5);
    Renderer::GetInstance().RenderShadowedText({ 675, 180 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40, "Club: ", 5);
    Renderer::GetInstance().RenderShadowedText({ 1280, 180 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40, "Position: ", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool SearchPlayers::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool SearchPlayers::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool SearchPlayers::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

SearchPlayers* SearchPlayers::GetAppState()
{
    static SearchPlayers appState;
    return &appState;
}
