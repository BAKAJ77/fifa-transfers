#include <states/new_save.h>
#include <states/main_menu.h>
#include <interface/menu_button.h>

void NewSave::Init()
{
    // Initialize the member variables
    this->goBackToPlayMenu = false;

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddStandaloneButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
}

void NewSave::Destroy() {}

void NewSave::Update(const float& deltaTime)
{
    if (!this->goBackToPlayMenu)
    {
        this->userInterface.Update(deltaTime);

        // Get the interface standalone buttons
        const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

        // Check and respond if any of the standalone buttons are pressed
        for (size_t index = 0; index < buttons.size(); index++)
        {
            const MenuButton* menuButton = (MenuButton*)buttons[index];
            if (menuButton->GetText() == "BACK" && menuButton->WasClicked())
                this->goBackToPlayMenu = true;
        }
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the interface fade out effect
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        if (this->userInterface.GetOpacity() == 0.0f)
        {
            // Update title logo fade in effect
            MainMenu::GetAppState()->SetLogoOpacity(std::min(MainMenu::GetAppState()->GetLogoOpacity() + (transitionSpeed * deltaTime), 255.0f));
            if (MainMenu::GetAppState()->GetLogoOpacity() == 255.0f)
                this->PopState();
        }
    }
}

void NewSave::Render() const
{
    this->userInterface.Render();
}

bool NewSave::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update title logo fade out effect
    MainMenu::GetAppState()->SetLogoOpacity(std::max(MainMenu::GetAppState()->GetLogoOpacity() - (transitionSpeed * deltaTime), 0.0f));

    // Once the title logo fade out effect is complete, update interface fade in effect
    if (MainMenu::GetAppState()->GetLogoOpacity() == 0.0f)
    {
        this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
        if (this->userInterface.GetOpacity() == 255.0f)
            return true;
    }

    return false;
}

NewSave* NewSave::GetAppState()
{
    static NewSave appState;
    return &appState;
}
