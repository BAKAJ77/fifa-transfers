#include <states/main_game.h>
#include <states/main_menu.h>
#include <states/save_writing.h>
#include <states/continue_game.h>

#include <interface/menu_button.h>

void MainGame::Init()
{
    this->updateWhilePaused = true;

    // Initialize the member variables
    this->changeParallelState = this->savingProgress = false;
    this->bkgOpacity = 0.0f;

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    this->userInterface.AddButton(new MenuButton({ 520, 60 }, { 400, 120 }, { 400, 120 }, "CONTINUE", glm::vec3(50), glm::vec3(90), glm::vec3(70), 
        255, true));
    this->userInterface.AddButton(new MenuButton({ 920, 60 }, { 400, 120 }, { 400, 120 }, "MY CLUB", glm::vec3(50), glm::vec3(90), glm::vec3(70), 
        255, true));
    this->userInterface.AddButton(new MenuButton({ 1320, 60 }, { 400, 120 }, { 400, 120 }, "SAVE", glm::vec3(50), glm::vec3(90), glm::vec3(70), 
        255, true));
    this->userInterface.AddButton(new MenuButton({ 1720, 60 }, { 400, 120 }, { 400, 120 }, "EXIT", glm::vec3(50), glm::vec3(90), glm::vec3(70), 
        255, true));

    // Push the default parallel app state
    this->PushState(ContinueGame::GetAppState());
    this->currentParallelState = ContinueGame::GetAppState();
}

void MainGame::Destroy() {}

void MainGame::Update(const float& deltaTime)
{
    // Update the user interface
    if (!this->savingProgress)
        this->userInterface.Update(deltaTime);

    // Check if any of the main buttons have been clicked
    for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
    {
        const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];

        if (button->GetText() == "CONTINUE" && button->WasClicked() && this->currentParallelState != ContinueGame::GetAppState()) // CONTINUE button was clicked
        {
            this->changeParallelState = true;
            this->currentParallelState = ContinueGame::GetAppState();
        }
        else if (button->GetText() == "MY CLUB" && button->WasClicked()) // MY CLUB button was clicked
        {
            // TODO
        }
        else if (button->GetText() == "SAVE" && button->WasClicked()) // SAVE button was clicked
        {
            this->savingProgress = true;
            SaveWriting::GetAppState()->SetNextState(nullptr);
            this->PushState(SaveWriting::GetAppState());
        }
        else if (button->GetText() == "EXIT" && button->WasClicked()) // EXIT button was clicked
        {
            this->savingProgress = true;
            SaveWriting::GetAppState()->SetNextState(MainMenu::GetAppState());
            this->SwitchState(SaveWriting::GetAppState());
        }
        else
            this->savingProgress = false;
    }

    // Change parallel app state if requested and if this app state is current main active
    if (this->changeParallelState && this->GetActiveAppState() == this)
    {
        this->PushState(this->currentParallelState);
        this->changeParallelState = false;
    }
}

void MainGame::Render() const
{
    // Render the background
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 2"),
        { glm::vec3(255), this->bkgOpacity });

    // Render screen-wide bar at the top (this is where the buttons will be)
    Renderer::GetInstance().RenderSquare({ 960, 62.5f }, { 1920, 125 }, { glm::vec3(70), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderSquare({ 960, 59 }, { 1920, 120 }, { glm::vec3(50), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderSquare({ 317.5, 62.5f }, { 5, 125 }, { glm::vec3(70), this->userInterface.GetOpacity() });

    // Render the small title logo
    Renderer::GetInstance().RenderSquare({ 157, 60 }, { 240, 65 }, TextureLoader::GetInstance().GetTexture("Title Logo Small"),
        { glm::vec3(255), this->userInterface.GetOpacity() });

    // Render the user interface
    this->userInterface.Render();
}

bool MainGame::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update fade in effect of the background
    this->bkgOpacity = std::min(this->bkgOpacity + (transitionSpeed * deltaTime), 255.0f);

    if (this->bkgOpacity == 255.0f)
    {
        // Update the fade in effect of the user interface
        this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));

        if (this->userInterface.GetOpacity() == 255.0f)
            return true;
    }

    return false;
}

MainGame* MainGame::GetAppState()
{
    static MainGame appState;
    return &appState;
}

bool MainGame::ShouldChangeParallelState() const
{
    return this->changeParallelState;
}
