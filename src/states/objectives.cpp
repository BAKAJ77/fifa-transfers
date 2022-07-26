#include <states/objectives.h>
#include <states/main_game.h>
#include <interface/menu_button.h>
#include <serialization/save_data.h>

void Objectives::Init()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(false);

    // Initialize the member variables
    this->exitState = false;
    
    // Fetch the Bahnschrift Bold font and required textures
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");
    this->unknownCircleTex = TextureLoader::GetInstance().GetTexture("Unknown Circle");
    this->checkmarkCircleTex = TextureLoader::GetInstance().GetTexture("Checkmark Circle");
    this->crossCircleTex = TextureLoader::GetInstance().GetTexture("Cross Circle");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
}

void Objectives::Destroy()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(true);
}

void Objectives::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of othe buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "BACK" && button->WasClicked())
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

void Objectives::Render() const
{
    const std::vector<Club::Objective>& objectives = MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetObjectives();
    
    // Render the objectives background
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderShadowedText({ 1270, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "YOUR OBJECTIVES", 5);

    // Render the league objectives first
    Renderer::GetInstance().RenderShadowedText({ 60, 220 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 60, "LEAGUE OBJECTIVES:", 5);

    for (const Club::Objective& objective : objectives)
    {
        if (objective.compID < 1000) // The IDs of league competitions are always less than 1000
        {
            std::string objectiveText;

            if (objective.targetEndPosition == 1)
                objectiveText = "We expect you to win the league title this season.";
            else if (objective.targetEndPosition == 21)
                objectiveText = "We expect you to finish at least 21st this season.";
            else if (objective.targetEndPosition == 2 || objective.targetEndPosition == 22)
                objectiveText = "We expect you to finish at least " + std::to_string(objective.targetEndPosition) + "nd this season.";
            else if (objective.targetEndPosition == 3 || objective.targetEndPosition == 23)
                objectiveText = "We expect you to finish at least " + std::to_string(objective.targetEndPosition) + "rd this season.";
            else
                objectiveText = "We expect you to finish at least " + std::to_string(objective.targetEndPosition) + "th this season.";

            Renderer::GetInstance().RenderShadowedText({ 130, 275 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35, objectiveText, 5);

            // Render an objective status indicator
            this->RenderObjectiveStatusIndicator(objective, 260, true);
            break;
        }
    }

    // Render the domestic cup objectives
    Renderer::GetInstance().RenderShadowedText({ 60, 375 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 60, 
        "DOMESTIC CUP OBJECTIVES:", 5);

    float textOffsetY = 0.0f;
    for (const Club::Objective& objective : objectives)
    {
        if (objective.compID > 1000) // The IDs of domestic cup competitions are always larger than 1000
        {
            const KnockoutCup* domesticCup = SaveData::GetInstance().GetCup(objective.compID);
            std::string objectiveText;

            if (objective.targetEndPosition == domesticCup->GetRounds().size() + 1)
                objectiveText = "We expect you to win the " + std::string(domesticCup->GetName()) + " this season.";
            else if (domesticCup->GetRounds()[objective.targetEndPosition - 1].find("Round") != std::string::npos)
            {
                objectiveText = "We expect you to at least reach " + domesticCup->GetRounds()[objective.targetEndPosition - 1] + " in the " + 
                    domesticCup->GetName().data() + " this season.";
            }
            else
            {
                objectiveText = "We expect you to at least reach the " + domesticCup->GetRounds()[objective.targetEndPosition - 1] + " in the " +
                    domesticCup->GetName().data() + " this season.";
            }
            
            Renderer::GetInstance().RenderShadowedText({ 130, 430 + textOffsetY }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35, 
                objectiveText, 5);

            // Render an objective status indicator
            this->RenderObjectiveStatusIndicator(objective, 417.5f + textOffsetY, false);
            textOffsetY += 60;
        }
    }

    // Render the user interface
    this->userInterface.Render();
}

void Objectives::RenderObjectiveStatusIndicator(const Club::Objective& objective, float yPos, bool isLeagueObjective) const
{
    for (const UserProfile::CompetitionData& compStats : MainGame::GetAppState()->GetCurrentUser()->GetCompetitionData())
    {
        if (compStats.compID == objective.compID)
        {
            if (compStats.seasonEndPosition != 0)
            {
                if (isLeagueObjective)
                {
                    if (compStats.seasonEndPosition <= objective.targetEndPosition) // CHECKMARK if the objective is successfully completed
                    {
                        Renderer::GetInstance().RenderSquare({ 85, yPos }, glm::vec2(50), this->checkmarkCircleTex, { glm::vec3(255),
                            this->userInterface.GetOpacity() });
                    }
                    else // CROSS if the objective was failed
                    {
                        Renderer::GetInstance().RenderSquare({ 85, yPos }, glm::vec2(50), this->crossCircleTex, { glm::vec3(255),
                            this->userInterface.GetOpacity() });
                    }
                }
                else
                {
                    if (compStats.seasonEndPosition >= objective.targetEndPosition) // CHECKMARK if the objective is successfully completed
                    {
                        Renderer::GetInstance().RenderSquare({ 85, yPos }, glm::vec2(50), this->checkmarkCircleTex, { glm::vec3(255),
                            this->userInterface.GetOpacity() });
                    }
                    else // CROSS if the objective was failed
                    {
                        Renderer::GetInstance().RenderSquare({ 85, yPos }, glm::vec2(50), this->crossCircleTex, { glm::vec3(255),
                            this->userInterface.GetOpacity() });
                    }
                }
            }
            else // UNKNOWN if the objective result hasn't been finalised yet
            {
                Renderer::GetInstance().RenderSquare({ 85, yPos }, glm::vec2(50), this->unknownCircleTex, { glm::vec3(255),
                    this->userInterface.GetOpacity() });
            }

            break;
        }
    }
}

bool Objectives::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

Objectives* Objectives::GetAppState()
{
    static Objectives appStates;
    return &appStates;
}
