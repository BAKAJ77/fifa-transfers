#include <states/inbox_menu.h>
#include <states/main_game.h>
#include <states/inbox_interface.h>

#include <interface/menu_button.h>

void InboxMenu::Init()
{
    // Initialize member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font and circle texture
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");
    this->circleTex = TextureLoader::GetInstance().GetTexture("Circle");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    this->userInterface.AddButton(new MenuButton({ 485, 525 }, { 920, 800 }, { 930, 810 }, "GENERAL INBOX", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 85, 20));
    this->userInterface.AddButton(new MenuButton({ 1435, 525 }, { 920, 800 }, { 930, 810 }, "TRANSFER INBOX", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 85, 20));

    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

}

void InboxMenu::Destroy() {}

void InboxMenu::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "GENERAL INBOX" && button->WasClicked())
            {
                InboxInterface::GetAppState()->SetInboxType(InboxInterface::InboxType::GENERAL);
                this->PushState(InboxInterface::GetAppState());
            }
            else if (button->GetText() == "TRANSFER INBOX" && button->WasClicked())
            {
                InboxInterface::GetAppState()->SetInboxType(InboxInterface::InboxType::TRANSFERS);
                this->PushState(InboxInterface::GetAppState());
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->PopState();
    }
}

void InboxMenu::Render() const
{
    // Render the app state title
    Renderer::GetInstance().RenderShadowedText({ 1450, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "INBOX MENU", 5);

    // Render the user interface
    this->userInterface.Render();

    // Render inbox message count indicators
    const size_t totalGeneralMessages = MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetGeneralMessages().size();
    const size_t totalTransferMessages = MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferMessages().size();
    
    if (totalGeneralMessages > 0)
    {
        Renderer::GetInstance().RenderSquare({ 850, 220 }, { 100, 100 }, this->circleTex, { glm::vec3(255), this->userInterface.GetOpacity() });

        const glm::vec2 textSize = Renderer::GetInstance().GetTextSize(this->font, 50, std::to_string(totalGeneralMessages));
        Renderer::GetInstance().RenderText({ 850 - (textSize.x / 2), 220 + (textSize.y / 2) }, { glm::vec3(255), this->userInterface.GetOpacity() }, 
            this->font, 50, std::to_string(totalGeneralMessages));
    }

    if (totalTransferMessages > 0)
    {
        Renderer::GetInstance().RenderSquare({ 1800, 220 }, { 100, 100 }, this->circleTex, { glm::vec3(255), this->userInterface.GetOpacity() });

        const glm::vec2 textSize = Renderer::GetInstance().GetTextSize(this->font, 50, std::to_string(totalTransferMessages));
        Renderer::GetInstance().RenderText({ 1800 - (textSize.x / 2), 220 + (textSize.y / 2) }, { glm::vec3(255), this->userInterface.GetOpacity() },
            this->font, 50, std::to_string(totalTransferMessages));
    }
}

bool InboxMenu::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool InboxMenu::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool InboxMenu::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

InboxMenu* InboxMenu::GetAppState()
{
    static InboxMenu appState;
    return &appState;
}
