#include <states/play_menu.h>
#include <states/new_save.h>
#include <states/load_save.h>

#include <interface/menu_button.h>

void PlayMenu::Init()
{
    // Initialize the member variables
    this->goBackToMainMenu = false;

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddStandaloneButton(new MenuButton({ 640, 750 }, { 450, 200 }, { 465, 215 }, "NEW SAVE"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1110, 750 }, { 450, 200 }, { 465, 215 }, "LOAD SAVE"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1580, 750 }, { 450, 200 }, { 465, 215 }, "BACK"));
}

void PlayMenu::Destroy() {}

void PlayMenu::Update(const float& deltaTime)
{
    if (!this->goBackToMainMenu)
    {
        this->userInterface.Update(deltaTime);

        // Get the interface standalone buttons
        const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

        // Check and respond if any of the standalone buttons are pressed
        for (size_t index = 0; index < buttons.size(); index++)
        {
            const MenuButton* menuButton = (MenuButton*)buttons[index];

            if (menuButton->GetText() == "NEW SAVE" && menuButton->WasClicked())
                this->PushState(NewSave::GetAppState());
            else if (menuButton->GetText() == "LOAD SAVE" && menuButton->WasClicked())
                this->PushState(LoadSave::GetAppState());
            else if (menuButton->GetText() == "BACK" && menuButton->WasClicked())
                this->goBackToMainMenu = true;
        }
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->PopState();
    }
}

void PlayMenu::Render() const
{
    this->userInterface.Render();
}

bool PlayMenu::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1500.0f;
    static bool startedTransition = false, finishedMovingButtons = false;

    // Get the interface standalone buttons
    const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

    // Set the position of the buttons towards the right of the screen
    if (!startedTransition)
    {
        for (size_t i = 0; i < buttons.size(); i++)
            buttons[i]->SetPosition({ 635 + (475 * i), 750 });

        startedTransition = true;
    }

    // Move the buttons towards the left of the screen
    for (size_t i = 0; i < buttons.size(); i++)
        buttons[i]->SetPosition({ std::max(buttons[i]->GetPosition().x - ((transitionSpeed / 3.0f) * deltaTime), 490.0f + (470.0f * i)),
            buttons[i]->GetPosition().y });

    if (buttons[0]->GetPosition().x == 490)
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

bool PlayMenu::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1500.0f;

    // Get the interface standalone buttons
    const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

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

bool PlayMenu::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

PlayMenu* PlayMenu::GetAppState()
{
    static PlayMenu appState;
    return &appState;
}
