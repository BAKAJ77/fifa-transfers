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
                if (this->competitionIndex == (int)this->displayedUser->GetCompetitionData().size())
                    this->competitionIndex = 0;
            }
            else if (button->GetText() == "PREVIOUS" && button->WasClicked())
            {
                --this->competitionIndex;
                if (this->competitionIndex == -1)
                    this->competitionIndex = (int)this->displayedUser->GetCompetitionData().size() - 1;
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

    if (compStats.compID >= 1000)
    {
        const KnockoutCup* cup = SaveData::GetInstance().GetCup(compStats.compID);
        Renderer::GetInstance().RenderShadowedText({ 60, 190 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
            std::string("COMPETITION NAME: ") + cup->GetName().data(), 5);

        if (compStats.seasonEndPosition != 0)
        {
            if (compStats.seasonEndPosition == cup->GetRounds().size() + 1)
            {
                Renderer::GetInstance().RenderShadowedText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
                    "SEASON ACHIEVEMENT: Cup Winner", 5);
            }
            else
            {
                Renderer::GetInstance().RenderShadowedText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
                    std::string("SEASON ACHIEVEMENT: ") + cup->GetRounds()[compStats.seasonEndPosition - 1], 5);
            }
        }
        else
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
                "SEASON ACHIEVEMENT: N/A", 5);
        }
    }
    else
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 190 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
            std::string("COMPETITION NAME: ") + SaveData::GetInstance().GetLeague(compStats.compID)->GetName().data(), 5);

        if (compStats.seasonEndPosition != 0)
        {
            std::string seasonAchievementString;

            if (compStats.seasonEndPosition == 1)
                seasonAchievementString = "SEASON ACHIEVEMENT: League Title Winner";
            else if (compStats.seasonEndPosition == 21)
                seasonAchievementString = "SEASON ACHIEVEMENT: " + std::to_string(compStats.seasonEndPosition) + "st";
            else if (compStats.seasonEndPosition == 2 || compStats.seasonEndPosition == 22)
                seasonAchievementString = "SEASON ACHIEVEMENT: " + std::to_string(compStats.seasonEndPosition) + "nd";
            else if (compStats.seasonEndPosition == 3 || compStats.seasonEndPosition == 23)
                seasonAchievementString = "SEASON ACHIEVEMENT: " + std::to_string(compStats.seasonEndPosition) + "rd";
            else
                seasonAchievementString = "SEASON ACHIEVEMENT: " + std::to_string(compStats.seasonEndPosition) + "th";

            if (compStats.wonPlayoffs)
                seasonAchievementString += " (Playoffs Title Winner)";

            Renderer::GetInstance().RenderShadowedText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28, seasonAchievementString, 5);
        }
        else
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 235 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
                "SEASON ACHIEVEMENT: N/A", 5);
        }
    }

    std::string titlesWonString = "TOTAL TITLES WON: " + std::to_string(compStats.titlesWon);
    if (compStats.playoffsWon > 0)
        titlesWonString += " (Including " + std::to_string(compStats.playoffsWon) + " Playoffs Titles)";

    Renderer::GetInstance().RenderShadowedText({ 60, 280 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28, titlesWonString, 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 345 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "CURRENT AMOUNT OF GOALS SCORED: " + std::to_string(compStats.currentScored), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "CURRENT AMOUNT OF GOALS CONCEDED: " + std::to_string(compStats.currentConceded), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 435 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "CURRENT AMOUNT OF GAMES WON: " + std::to_string(compStats.currentWins), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 480 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "CURRENT AMOUNT OF GAMES DRAWN: " + std::to_string(compStats.currentDraws), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 525 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "CURRENT AMOUNT OF GAMES LOST: " + std::to_string(compStats.currentLosses), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 590 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "MOST AMOUNT OF GOALS SCORED: " + std::to_string(compStats.mostScored), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 635 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "MOST AMOUNT OF GOALS CONCEDED: " + std::to_string(compStats.mostConceded), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 680 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "MOST AMOUNT OF GAMES WON: " + std::to_string(compStats.mostWins), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 725 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "MOST AMOUNT OF GAMES DRAWN: " + std::to_string(compStats.mostDraws), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 770 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "MOST AMOUNT OF GAMES LOST: " + std::to_string(compStats.mostLosses), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 835 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "TOTAL AMOUNT OF GOALS SCORED: " + std::to_string(compStats.totalScored), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 880 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "TOTAL AMOUNT OF GOALS CONCEDED: " + std::to_string(compStats.totalConceded), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 925 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "TOTAL AMOUNT OF GAMES WON: " + std::to_string(compStats.totalWins), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 970 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "TOTAL AMOUNT OF GAMES DRAWN: " + std::to_string(compStats.totalDraws), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 1015 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 28,
        "TOTAL AMOUNT OF GAMES LOST: " + std::to_string(compStats.totalLosses), 5);

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
