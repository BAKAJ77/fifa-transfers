#include <states/my_club_section.h>
#include <states/main_game.h>
#include <interface/menu_button.h>
#include <serialization/save_data.h>

void MyClub::Init()
{
    // Initialize the member variables
    this->currentUser = &SaveData::GetInstance().GetUsers().front();

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    
    this->userInterface.AddButton(new MenuButton({ 275, 417.5f }, { 500, 235 }, { 510, 245 }, "MANAGE SQUAD", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255.0f, false, 50));
    this->userInterface.AddButton(new MenuButton({ 275, 677.5f }, { 500, 235 }, { 510, 245 }, "TRAINING", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255.0f, false, 50));
    this->userInterface.AddButton(new MenuButton({ 275, 937.5f }, { 500, 235 }, { 510, 245 }, "OBJECTIVES", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255.0f, false, 50));
    this->userInterface.AddButton(new MenuButton({ 800, 417.5f }, { 500, 235 }, { 510, 245 }, "INBOX", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255.0f, false, 50));
    this->userInterface.AddButton(new MenuButton({ 800, 677.5f }, { 500, 235 }, { 510, 245 }, "PLAYER SEARCH", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255.0f, false, 50));
    this->userInterface.AddButton(new MenuButton({ 800, 937.5f }, { 500, 235 }, { 510, 245 }, "STATISTICS", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255.0f, false, 50));
    this->userInterface.AddButton(new MenuButton({ 1485, 677.5f }, { 820, 755 }, { 830, 765 }, "SWITCH USER", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255.0f, false, 50));
}

void MyClub::Destroy() {}

void MyClub::Update(const float& deltaTime)
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

void MyClub::Render() const
{
    // Render the user interface
    this->userInterface.Render();

    // Render text detailing the current user
    Renderer::GetInstance().RenderShadowedText({ 25, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        std::string("CURRENT USER: ") + this->currentUser->GetName().data(), 5);

    Renderer::GetInstance().RenderShadowedText({ 25, 265 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        std::string("CLUB: ") + this->currentUser->GetClub()->GetName().data(), 5);
}

bool MyClub::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));

    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool MyClub::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));

    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool MyClub::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

MyClub* MyClub::GetAppState()
{
    static MyClub appState;
    return &appState;
}
