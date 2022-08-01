#include <states/inbox_interface.h>
#include <states/main_game.h>
#include <serialization/club_entity.h>
#include <interface/menu_button.h>

void InboxInterface::Init()
{
    // Initialize member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
    
    if (this->type == InboxType::GENERAL)
        this->userInterface.AddButton(new MenuButton({ 1410, 1005 }, { 300, 100 }, { 315, 115 }, "CLEAR"));

    this->userInterface.AddSelectionList("Inbox Messages", { { 960, 490 }, { 1860, 720 }, 80, 255, 25 });
    this->userInterface.GetSelectionList("Inbox Messages")->AddCategory("Inbox Messages");

    // Load the general/transfer messages
    if (this->type == InboxType::GENERAL)
    {
        for (const std::string& generalMsg : MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetGeneralMessages())
            this->userInterface.GetSelectionList("Inbox Messages")->AddElement({ generalMsg }, -1);
    }
}

void InboxInterface::Destroy() {}

void InboxInterface::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];

            if (button->GetText() == "CLEAR" && button->WasClicked())
            {
                MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetGeneralMessages().clear();
                this->userInterface.GetSelectionList("Inbox Messages")->Clear();
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
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

void InboxInterface::Render() const
{
    // Render the app state title
    if (this->type == InboxType::GENERAL)
    {
        Renderer::GetInstance().RenderShadowedText({ 1320, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
            "GENERAL INBOX", 5);
    }
    else
    {
        Renderer::GetInstance().RenderShadowedText({ 1280, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
            "TRANSFER INBOX", 5);
    }

    // Render the user interface
    this->userInterface.Render();
}

bool InboxInterface::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

InboxInterface* InboxInterface::GetAppState()
{
    static InboxInterface appState;
    return &appState;
}

void InboxInterface::SetInboxType(InboxType type)
{
    this->type = type;
}
