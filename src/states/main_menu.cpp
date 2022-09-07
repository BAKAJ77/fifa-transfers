#include <states/main_menu.h>
#include <states/play_menu.h>
#include <states/settings.h>

#include <interface/menu_button.h>

void MainMenu::Init()
{
    // Initialize the member variables
    this->bkgOpacity = this->logoOpacity = 0.0f;
    this->paused = false;

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    this->userInterface.AddButton(new MenuButton({ 485, 750 }, { 450, 200 }, { 465, 215 }, "PLAY"));
    this->userInterface.AddButton(new MenuButton({ 955, 750 }, { 450, 200 }, { 465, 215 }, "SETTINGS"));
    this->userInterface.AddButton(new MenuButton({ 1425, 750 }, { 450, 200 }, { 465, 215 }, "EXIT"));
}

void MainMenu::Destroy() {}

void MainMenu::Update(const float& deltaTime)
{
    // Update the menu user interface
    this->userInterface.Update(deltaTime);

    // Check and respond if any of the standalone buttons are pressed
    const std::vector<ButtonBase*>& buttons = this->userInterface.GetButtons();

    for (size_t index = 0; index < buttons.size(); index++)
    {
        const MenuButton* menuButton = (MenuButton*)buttons[index];
        if (menuButton->GetText() == "PLAY" && menuButton->WasClicked())
            this->PushState(PlayMenu::GetAppState());
        else if (menuButton->GetText() == "SETTINGS" && menuButton->WasClicked())
            this->PushState(Settings::GetAppState());
        else if (menuButton->GetText() == "EXIT" && menuButton->WasClicked())
            this->PopState();
    }
}

void MainMenu::Render() const
{
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 2"),
        { 255, 255, 255, this->bkgOpacity });
    Renderer::GetInstance().RenderSquare({ 960, 400 }, { 1280, 345 }, TextureLoader::GetInstance().GetTexture("Title Logo Large"),
        { 255, 255, 255, this->logoOpacity });

    this->userInterface.Render();
}

bool MainMenu::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update background fade in effect
    this->logoOpacity = this->bkgOpacity = std::min(this->bkgOpacity + ((transitionSpeed / 2.0f) * deltaTime), 255.0f);

    // Once background fade in effect is complete, update the gui and logo fade in effect
    if (this->bkgOpacity == 255.0f)
    {
        this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
        if (this->userInterface.GetOpacity() == 255.0f)
            return true;
    }

    return false;
}

bool MainMenu::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1500.0f;

    // Get the interface standalone buttons
    const std::vector<ButtonBase*>& buttons = this->userInterface.GetButtons();
    
    // Move the buttons towards the left of the screen
    for (size_t index = 0; index < buttons.size(); index++)
    {
        MenuButton* menuButton = (MenuButton*)buttons[index];
        menuButton->SetPosition({ menuButton->GetPosition().x - ((transitionSpeed / 3.0f) * deltaTime), menuButton->GetPosition().y });
    }

    // Animate the interface fading away
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool MainMenu::OnResumeTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1500.0f;
    static bool startedTransition = false, finishedMovingButtons = false;

    // Get the interface standalone buttons
    const std::vector<ButtonBase*>& buttons = this->userInterface.GetButtons();

    // Set the position of the buttons towards the right of the screen
    if (!startedTransition)
    {
        for (size_t i = 0; i < buttons.size(); i++)
            buttons[i]->SetPosition({ 635 + (470 * i), 750 });

        startedTransition = true;
    }

    // Move the buttons towards the left of the screen
    for (size_t i = 0; i < buttons.size(); i++)
        buttons[i]->SetPosition({ std::max(buttons[i]->GetPosition().x - ((transitionSpeed / 3.0f) * deltaTime), 485.0f + (470.0f * i)), 
            buttons[i]->GetPosition().y });

    if (buttons[0]->GetPosition().x == 485)
        finishedMovingButtons = true;

    // Animate the interface fading away
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f && finishedMovingButtons)
    {
        startedTransition = false;
        finishedMovingButtons = false;

        return true;
    }

    return false;
}

void MainMenu::SetLogoOpacity(float opacity)
{
    this->logoOpacity = opacity;
}

float MainMenu::GetLogoOpacity() const
{
    return this->logoOpacity;
}

MainMenu* MainMenu::GetAppState()
{
    static MainMenu appState;
    return &appState;
}
