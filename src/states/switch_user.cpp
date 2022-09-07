#include <states/switch_user.h>
#include <states/main_game.h>
#include <interface/menu_button.h>
#include <serialization/save_data.h>

void SwitchUser::Init()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(false);

    // Initialize the member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddSelectionList("Users", { { 797.5f, 520 }, { 1545, 640 }, 80 });
    this->userInterface.GetSelectionList("Users")->AddCategory("User");
    this->userInterface.GetSelectionList("Users")->AddCategory("Club");

    for (size_t index = 0; index < SaveData::GetInstance().GetUsers().size(); index++)
    {
        const UserProfile& user = SaveData::GetInstance().GetUsers()[index];

        if (user.GetID() != MainGame::GetAppState()->GetCurrentUser()->GetID())
            this->userInterface.GetSelectionList("Users")->AddElement({ user.GetName().data(), user.GetClub()->GetName().data() }, (int)index);
    }
}

void SwitchUser::Destroy()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(true);
}

void SwitchUser::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        // Check if a user profile has been selected
        const int selectedUserIndex = this->userInterface.GetSelectionList("Users")->GetCurrentSelected();
        if (selectedUserIndex != -1)
        {
            MainGame::GetAppState()->SetCurrentUser(SaveData::GetInstance().GetUsers()[selectedUserIndex]);
            this->exitState = true;
        }
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the fade out effect of the user interface
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        if (this->userInterface.GetOpacity() == 0.0f)
            this->PopState();
    }
}

void SwitchUser::Render() const
{
    // Render the text description
    Renderer::GetInstance().RenderShadowedText({ 30, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Select the user profile you want to switch to below", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool SwitchUser::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

SwitchUser* SwitchUser::GetAppState()
{
    static SwitchUser appState;
    return &appState;
}
