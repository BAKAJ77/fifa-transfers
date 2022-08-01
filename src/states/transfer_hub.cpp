#include <states/transfer_hub.h>
#include <states/main_game.h>
#include <states/search_players.h>
#include <states/transfer_history.h>

#include <interface/menu_button.h>
#include <util/data_manip.h>

void TransferHub::Init()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(false);

    // Initialize the member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    this->userInterface.AddButton(new MenuButton({ 485, 590 }, { 920, 670 }, { 930, 680 }, "SEARCH FOR PLAYER", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 85, 20));
    this->userInterface.AddButton(new MenuButton({ 1435, 590 }, { 920, 670 }, { 930, 680 }, "TRANSFER HISTORY", glm::vec3(60), glm::vec3(90),
        glm::vec3(120), 255, false, 85, 20));

    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
}

void TransferHub::Destroy()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(true);
}

void TransferHub::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of other buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "SEARCH FOR PLAYER" && button->WasClicked())
                this->PushState(SearchPlayers::GetAppState());
            else if (button->GetText() == "TRANSFER HISTORY" && button->WasClicked())
            {
                this->PushState(TransferHistory::GetAppState());
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

void TransferHub::Render() const
{
    // Render the transfer hub app state title
    Renderer::GetInstance().RenderShadowedText({ 1350, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "TRANSFER HUB", 5);

    // Render the background bar then the transfer and wage budget text on top
    Renderer::GetInstance().RenderSquare({ 960, 180 }, { 1870, 100 }, { glm::vec3(30), this->userInterface.GetOpacity() });

    const glm::vec2 wageBudgetTextSize = Renderer::GetInstance().GetTextSize(this->font, 50, "WAGE BUDGET: " +
        Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetWageBudget()));

    Renderer::GetInstance().RenderShadowedText({ 55, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() },
        this->font, 50, "TRANSFER BUDGET: " + Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTransferBudget()), 5);
    Renderer::GetInstance().RenderShadowedText({ 1895 - wageBudgetTextSize.x, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() },
        this->font, 50, "WAGE BUDGET: " + Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetWageBudget()), 5);

    // Render the user interface
    this->userInterface.Render();
}

bool TransferHub::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool TransferHub::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool TransferHub::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

TransferHub* TransferHub::GetAppState()
{
    static TransferHub appState;
    return &appState;
}
