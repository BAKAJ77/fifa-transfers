#include <states/continue_game.h>
#include <states/main_game.h>
#include <states/end_competition.h>
#include <states/end_season.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>

void ContinueGame::Init()
{
    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    this->userInterface.AddButton(new MenuButton({ 960, 482.5f }, { 1870, 365 }, { 1880, 375 }, "END COMPETITION", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 120, 50));
    this->userInterface.AddButton(new MenuButton({ 960, 872.5f }, { 1870, 365 }, { 1880, 375 }, "END SEASON", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 120, 50));
}

void ContinueGame::Destroy() {}

void ContinueGame::Update(const float& deltaTime)
{
    // Update the user interface
    this->userInterface.Update(deltaTime);

    // Check if any of the buttons have been clicked
    for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
    {
        const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
        if (button->GetText() == "END COMPETITION" && button->WasClicked())
            this->PushState(EndCompetition::GetAppState());
        else if (button->GetText() == "END SEASON" && button->WasClicked())
            this->PushState(EndSeason::GetAppState());
    }

    // If another parallel app state has been requested to start, then roll back to the main game state so it can be stared
    if (MainGame::GetAppState()->ShouldChangeParallelState())
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the fade in effect of the user interface
        this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
        if (this->userInterface.GetOpacity() == 255.0f)
            this->RollBack(MainGame::GetAppState());
    }
}

void ContinueGame::Render() const
{
    // Render the user interface
    this->userInterface.Render();

    // Render the current year and league text
    Renderer::GetInstance().RenderShadowedText({ 25, 265 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        std::string("CURRENT LEAGUE: ") + SaveData::GetInstance().GetCurrentLeague()->GetName().data(), 5);

    Renderer::GetInstance().RenderShadowedText({ 25, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "CURRENT YEAR: " + std::to_string(SaveData::GetInstance().GetCurrentYear()), 5);
}

bool ContinueGame::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool ContinueGame::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    MainGame::GetAppState()->GetUserInterface().SetOpacity(this->userInterface.GetOpacity());

    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool ContinueGame::OnResumeTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    MainGame::GetAppState()->GetUserInterface().SetOpacity(this->userInterface.GetOpacity());

    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

ContinueGame* ContinueGame::GetAppState()
{
    static ContinueGame appState;
    return &appState;
}
