#include <states/user_setup.h>
#include <interface/menu_button.h>

void UserSetup::Init()
{
    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddStandaloneButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
}

void UserSetup::Destroy()
{
}

void UserSetup::Update(const float& deltaTime)
{
    this->userInterface.Update(deltaTime);
}

void UserSetup::Render() const
{
    this->userInterface.Render();
}

bool UserSetup::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the interface fade in effect
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

UserSetup* UserSetup::GetAppState()
{
    static UserSetup appState;
    return &appState;
}
