#include <states/continue_game.h>
#include <states/main_game.h>
#include <interface/menu_button.h>

#include <serialization/save_data.h>

void ContinueGame::Init()
{
    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    this->userInterface.AddButton(new MenuButton({ 486.25f, 482.5f }, { 922.5f, 365 }, { 932.5f, 375 }, "END COMPETITION", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 90, 25));
    this->userInterface.AddButton(new MenuButton({ 486.25f, 872.5f }, { 922.5f, 365 }, { 932.5f, 375 }, "END SEASON", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 90, 25));

    this->userInterface.AddSelectionList("Transfer News", SelectionList({ 1433.75f, 550 }, { 922.5f, 800 }, 80));
    this->userInterface.GetSelectionList("Transfer News")->AddCategory("Transfer News");
}

void ContinueGame::Destroy() {}

void ContinueGame::Update(const float& deltaTime)
{
    // Update the user interface
    this->userInterface.Update(deltaTime);

    // If another parallel app state has been requested to start, then roll back to the main game state so it can be stared
    if (MainGame::GetAppState()->ShouldChangeParallelState())
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->RollBack(MainGame::GetAppState());
    }
}

void ContinueGame::Render() const
{
    // Render the user interface
    this->userInterface.Render();

    // Render the season count text
    Renderer::GetInstance().RenderShadowedText({ 25, 245 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 100, 
        "YEAR: " + std::to_string(SaveData::GetInstance().GetCurrentYear()), 5);
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

    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool ContinueGame::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

ContinueGame* ContinueGame::GetAppState()
{
    static ContinueGame appState;
    return &appState;
}
