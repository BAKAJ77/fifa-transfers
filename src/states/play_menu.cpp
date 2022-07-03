#include <states/play_menu.h>
#include <interface/menu_button.h>

void PlayMenu::Init()
{
    // Initialize the member variables
    this->goBackToMenu = false;

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddStandaloneButton(new MenuButton({ 635, 750 }, { 450, 200 }, { 465, 215 }, "NEW GAME"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1110, 750 }, { 450, 200 }, { 465, 215 }, "LOAD GAME"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1585, 750 }, { 450, 200 }, { 465, 215 }, "GO BACK"));
}

void PlayMenu::Destroy() {}

void PlayMenu::Update(const float& deltaTime)
{
    // Get the interface standalone buttons
    const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

    if (!this->goBackToMenu)
    {
        this->userInterface.Update(deltaTime);

        // Check and respond if any of the standalone buttons are pressed
        for (size_t index = 0; index < buttons.size(); index++)
        {
            const MenuButton* menuButton = (MenuButton*)buttons[index];
            if (menuButton->GetText() == "GO BACK" && menuButton->WasClicked())
                this->goBackToMenu = true;
        }
    }
    else
    {
        constexpr float transitionSpeed = 1500.0f;

        // Move the buttons towards the left of the screen
        for (size_t index = 0; index < buttons.size(); index++)
        {
            MenuButton* menuButton = (MenuButton*)buttons[index];
            menuButton->SetPosition({ menuButton->GetPosition().x - ((transitionSpeed / 3.0f) * deltaTime), menuButton->GetPosition().y });
        }

        // Animate the interface fading away
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        if (this->userInterface.GetOpacity() == 0.0f)
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
    static bool finishedMovingButtons = false;

    // Get the interface standalone buttons
    const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

    // Move the buttons towards the left of the screen
    buttons[0]->SetPosition({std::max(buttons[0]->GetPosition().x - ((transitionSpeed / 3.0f) * deltaTime),
        960 - buttons[0]->GetCurrentSize().x - 25), buttons[0]->GetPosition().y });
    
    buttons[1]->SetPosition({ std::max(buttons[1]->GetPosition().x - ((transitionSpeed / 3.0f) * deltaTime), 960.0f), buttons[1]->GetPosition().y });

    buttons[2]->SetPosition({ std::max(buttons[2]->GetPosition().x - ((transitionSpeed / 3.0f) * deltaTime), 
        960 + buttons[2]->GetCurrentSize().x + 25), buttons[2]->GetPosition().y });

    if (buttons[0]->GetPosition().x == 960 - buttons[0]->GetCurrentSize().x - 25)
        finishedMovingButtons = true;

    // Animate the interface fading away
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f && finishedMovingButtons)
    {
        finishedMovingButtons = false;
        return true;
    }

    return false;
}

PlayMenu* PlayMenu::GetAppState()
{
    static PlayMenu appState;
    return &appState;
}
