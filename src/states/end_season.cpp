#include <states/end_season.h>
#include <states/objectives.h>
#include <states/end_competition.h>
#include <states/financials_generation.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/timestamp.h>

void EndSeason::Init()
{
    // Initialized the member variables
    this->exitState = false;
    this->userIndex = 0;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    if (EndCompetition::GetAppState()->GetAmountOfIncompleteCompetitions() == 0)
    {
        this->userInterface.AddButton(new MenuButton({ 1745, 880 }, { 300, 100 }, { 315, 115 }, "OBJECTIVES"));
        this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "NEXT"));
    }
    else
        this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
}

void EndSeason::Destroy() {}

void EndSeason::Update(const float& deltaTime)
{   
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of othe buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "OBJECTIVES" && button->WasClicked())
            {
                Objectives::GetAppState()->SetUserProfile(&SaveData::GetInstance().GetUsers()[this->userIndex], false);
                this->PushState(Objectives::GetAppState());
            }
            else if (button->GetText() == "NEXT" && button->WasClicked())
            {
                if ((this->userIndex + 1) == (int)SaveData::GetInstance().GetUsers().size())
                    this->PushState(FinancialsGeneration::GetAppState());
                else
                    ++this->userIndex;
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

void EndSeason::Render() const
{
    if (EndCompetition::GetAppState()->GetAmountOfIncompleteCompetitions() == 0)
    {
        const UserProfile& user = SaveData::GetInstance().GetUsers()[this->userIndex];

        // Render the background and app state title
        Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });

        const glm::vec2 titleTextSize = Renderer::GetInstance().GetTextSize(this->font, 75, std::string(user.GetName()) + "'s SEASON OVERVIEW");
        Renderer::GetInstance().RenderShadowedText({ 1920 - titleTextSize.x, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
            std::string(user.GetName()) + "'s SEASON OVERVIEW", 5);

        // Render the overall stats (e.g. overall goals scored, goals conceded etc)
        int totalScored = 0, totalConceded = 0, totalWins = 0, totalDraws = 0, totalLosses = 0;
        for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
        {
            totalScored += compStats.currentScored;
            totalConceded += compStats.currentConceded;
            totalWins += compStats.currentWins;
            totalDraws += compStats.currentDraws;
            totalLosses += compStats.currentLosses;
        }

        Renderer::GetInstance().RenderShadowedText({ 60, 220 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 60, "OVERALL STATS", 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 280 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            "- Total amount of goals scored this season: " + std::to_string(totalScored), 5);
        Renderer::GetInstance().RenderShadowedText({ 60, 340 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            "- Total amount of goals conceded this season: " + std::to_string(totalConceded), 5);
        Renderer::GetInstance().RenderShadowedText({ 60, 400 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            "- Total amount of games won this season: " + std::to_string(totalWins), 5);
        Renderer::GetInstance().RenderShadowedText({ 60, 460 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            "- Total amount of games drawn this season: " + std::to_string(totalDraws), 5);
        Renderer::GetInstance().RenderShadowedText({ 60, 520 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            "- Total amount of games lost this season: " + std::to_string(totalLosses), 5);

        // Render the competition achievement stats (e.g. final league position, won the domestic cup or reached certain round)
        std::string achievementText;
        float textOffsetY = 0.0f;

        Renderer::GetInstance().RenderShadowedText({ 60, 620 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 60, 
            "COMPETITION ACHIEVEMENTS", 5);

        for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
        {
            if (compStats.compID < 1000)
            {
                const League* league = SaveData::GetInstance().GetCurrentLeague();

                if (compStats.seasonEndPosition == 1)
                    achievementText = "- You've managed to win the " + std::string(league->GetName()) + " title this season.";
                else if (compStats.seasonEndPosition == 21)
                    achievementText = "- You finished 21st in the " + std::string(league->GetName()) + " this season.";
                else if (compStats.seasonEndPosition == 2 || compStats.seasonEndPosition == 22)
                {
                    achievementText = "- You finished " + std::to_string(compStats.seasonEndPosition) + "nd in the " + league->GetName().data() + 
                        " this season.";
                }
                else if (compStats.seasonEndPosition == 3 || compStats.seasonEndPosition == 23)
                {
                    achievementText = "- You finished " + std::to_string(compStats.seasonEndPosition) + "rd in the " + league->GetName().data() +
                        " this season.";
                }
                else
                {
                    achievementText = "- You finished " + std::to_string(compStats.seasonEndPosition) + "th in the " + league->GetName().data() +
                        " this season.";
                }
            }
            else
            {
                if (compStats.seasonEndPosition != 0)
                {
                    const KnockoutCup* domesticCup = SaveData::GetInstance().GetCup(compStats.compID);

                    if (compStats.seasonEndPosition == domesticCup->GetRounds().size() + 1)
                        achievementText = "- You've managed to win the " + std::string(domesticCup->GetName()) + " this season.";
                    else if (domesticCup->GetRounds()[compStats.seasonEndPosition - 1].find("Round") != std::string::npos)
                    {
                        achievementText = "- You got knocked out in " + domesticCup->GetRounds()[compStats.seasonEndPosition - 1] + " of the " +
                            domesticCup->GetName().data() + " this season.";
                    }
                    else
                    {
                        achievementText = "- You got knocked out in the " + domesticCup->GetRounds()[compStats.seasonEndPosition - 1] + " of the " +
                            domesticCup->GetName().data() + " this season.";
                    }
                }
            }

            Renderer::GetInstance().RenderShadowedText({ 60, 680 + textOffsetY }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font,
                35, achievementText, 5);

            textOffsetY += 60;
        }
    }
    else
    {
        // The user has not completed all the required competitions yet therefore they can't advance, so let them know
        Renderer::GetInstance().RenderShadowedText({ 30, 1000 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 40,
            "Make sure you've completed competitions shown in the END COMPETITION section.", 5);
        Renderer::GetInstance().RenderShadowedText({ 30, 1050 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 40,
            "The following competitions: UCL, UEL and UCONFL aren't required to be completed.", 5);
    }

    // Render the user interface
    this->userInterface.Render();
}

bool EndSeason::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool EndSeason::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool EndSeason::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

EndSeason* EndSeason::GetAppState()
{
    static EndSeason appState;
    return &appState;
}
