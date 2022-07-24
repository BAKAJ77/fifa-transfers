#include <states/statistics.h>
#include <states/main_game.h>
#include <interface/menu_button.h>
#include <serialization/save_data.h>

void Statistics::Init()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(false);

    // Initialize the member variables
    this->exitState = false;
    this->competitionIndex = 0;
    this->displayedUser = MainGame::GetAppState()->GetCurrentUser();

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    if (this->displayedUser->GetCompetitionData().size() > 1)
    {
        this->userInterface.AddButton(new MenuButton({ 1745, 755 }, { 300, 100 }, { 315, 115 }, "NEXT"));
        this->userInterface.AddButton(new MenuButton({ 1745, 880 }, { 300, 100 }, { 315, 115 }, "PREVIOUS"));
    }
}

void Statistics::Destroy()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(true);
}

void Statistics::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of othe buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "NEXT" && button->WasClicked())
            {
                ++this->competitionIndex;
                if (this->competitionIndex == this->displayedUser->GetCompetitionData().size())
                    this->competitionIndex = 0;
            }
            else if (button->GetText() == "PREVIOUS" && button->WasClicked())
            {
                --this->competitionIndex;
                if (this->competitionIndex == -1)
                    this->competitionIndex = this->displayedUser->GetCompetitionData().size() - 1;
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

void Statistics::Render() const
{
    // Render the statistics background
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderShadowedText({ 1300, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "YOUR STATISTICS", 5);

    // Render the statistics text
    const UserProfile::CompetitionData& compStats = this->displayedUser->GetCompetitionData()[this->competitionIndex];

    if (compStats.compID > 1000)
    {
        const KnockoutCup* cup = SaveData::GetInstance().GetCup(compStats.compID);
        Renderer::GetInstance().RenderText({ 60, 190 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
            std::string("COMPETITION NAME: ") + cup->GetName().data());

        if (compStats.seasonEndPosition != 0)
        {
            if (compStats.seasonEndPosition == cup->GetRounds().size() + 1)
            {
                Renderer::GetInstance().RenderText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
                    "SEASON ACHIEVEMENT: CUP WINNER");
            }
            else
            {
                Renderer::GetInstance().RenderText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
                    std::string("SEASON ACHIEVEMENT: ") + cup->GetRounds()[compStats.seasonEndPosition - 1]);
            }
        }
        else
        {
            Renderer::GetInstance().RenderText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30, 
                "SEASON ACHIEVEMENT: N/A");
        }
    }
    else
    {
        Renderer::GetInstance().RenderText({ 60, 190 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
            std::string("COMPETITION NAME: ") + SaveData::GetInstance().GetLeague(compStats.compID)->GetName().data());

        if (compStats.seasonEndPosition != 0)
        {
            Renderer::GetInstance().RenderText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
                "SEASON ACHIEVEMENT: " + std::to_string(compStats.seasonEndPosition));
        }
        else
        {
            Renderer::GetInstance().RenderText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
                "SEASON ACHIEVEMENT: N/A");
        }
    }

    Renderer::GetInstance().RenderText({ 60, 315 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "CURRENT AMOUNT OF GOALS SCORED: " + std::to_string(compStats.currentScored));

    Renderer::GetInstance().RenderText({ 60, 360 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "CURRENT AMOUNT OF GOALS CONCEDED: " + std::to_string(compStats.currentConceded));

    Renderer::GetInstance().RenderText({ 60, 405 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "CURRENT AMOUNT OF GAMES WON: " + std::to_string(compStats.currentWins));

    Renderer::GetInstance().RenderText({ 60, 450 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "CURRENT AMOUNT OF GAMES DRAWN: " + std::to_string(compStats.currentDraws));

    Renderer::GetInstance().RenderText({ 60, 495 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "CURRENT AMOUNT OF GAMES LOST: " + std::to_string(compStats.currentLosses));

    Renderer::GetInstance().RenderText({ 60, 575 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "MOST AMOUNT OF GOALS SCORED: " + std::to_string(compStats.mostScored));

    Renderer::GetInstance().RenderText({ 60, 620 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "MOST AMOUNT OF GOALS CONCEDED: " + std::to_string(compStats.mostConceded));

    Renderer::GetInstance().RenderText({ 60, 665 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "MOST AMOUNT OF GAMES WON: " + std::to_string(compStats.mostWins));

    Renderer::GetInstance().RenderText({ 60, 710 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "MOST AMOUNT OF GAMES DRAWN: " + std::to_string(compStats.mostDraws));

    Renderer::GetInstance().RenderText({ 60, 755 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "MOST AMOUNT OF GAMES LOST: " + std::to_string(compStats.mostLosses));

    Renderer::GetInstance().RenderText({ 60, 835 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "TOTAL AMOUNT OF GOALS SCORED: " + std::to_string(compStats.totalScored));

    Renderer::GetInstance().RenderText({ 60, 880 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "TOTAL AMOUNT OF GOALS CONCEDED: " + std::to_string(compStats.totalConceded));

    Renderer::GetInstance().RenderText({ 60, 925 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "TOTAL AMOUNT OF GAMES WON: " + std::to_string(compStats.totalWins));

    Renderer::GetInstance().RenderText({ 60, 970 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "TOTAL AMOUNT OF GAMES DRAWN: " + std::to_string(compStats.totalDraws));

    Renderer::GetInstance().RenderText({ 60, 1015 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
        "TOTAL AMOUNT OF GAMES LOST: " + std::to_string(compStats.totalLosses));

    // Render the user interface
    this->userInterface.Render();
}

bool Statistics::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

Statistics* Statistics::GetAppState()
{
    static Statistics appState;
    return &appState;
}
