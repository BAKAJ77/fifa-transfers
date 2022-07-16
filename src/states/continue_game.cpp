#include <states/continue_game.h>
#include <states/main_game.h>
#include <interface/menu_button.h>

void ContinueGame::Init()
{
    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 486.25f, 370 }, { 922.5f, 440 }, { 932.5f, 450 }, "END COMPETITION", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 90, 25));
    this->userInterface.AddButton(new MenuButton({ 486.25f, 835 }, { 922.5f, 440 }, { 932.5f, 450 }, "END SEASON", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 90, 25));
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
