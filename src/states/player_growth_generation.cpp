#include <states/player_growth_generation.h>
#include <states/new_season_setup.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/random_engine.h>
#include <util/data_manip.h>

void PlayerGrowthGeneration::Init()
{
    // Initialized the member variables
    this->userIndex = 0;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Begin the player growth operation
    for (UserProfile& user : SaveData::GetInstance().GetUsers())
    {
        // Tally up the amount of goals scored and conceded by the user's club
        int totalGoalsScored = 0, totalGoalsConceded = 0;
        for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
        {
            totalGoalsScored += compStats.currentScored;
            totalGoalsConceded += compStats.currentConceded;
        }

        // Calculate the amount of growth for each player
        for (Player* player : user.GetClub()->GetPlayers())
        {
            if (player->GetOverall() < player->GetPotential())
            {
                // Fetch the level of the training staff allocated to the player's position
                const int staffLevel = user.GetClub()->GetTrainingStaff((Club::StaffType)SaveData::GetInstance().GetPosition(player->GetPosition())->category).level;
                
                // Player growth is calculated differently based on whether the player is an attacking or defensive minded player
                int overallIncreaseAmount = 0;

                if (SaveData::GetInstance().GetPosition(player->GetPosition())->category == SaveData::PositionCategory::FORWARD ||
                   (SaveData::GetInstance().GetPosition(player->GetPosition())->category == SaveData::PositionCategory::MIDFIELDER && 
                       SaveData::GetInstance().GetPosition(player->GetPosition())->type.find("DM") == std::string::npos))
                {
                    // THIS IS FOR ATTACKING MINDED PLAYERS e.g. ST, LW, CAM, CM etc
                    const float min = (500.0f + totalGoalsScored) * 1.5f;
                    const float max = (1000.0f + totalGoalsScored) * 1.5f;
                    const float generatedWeight = RandomEngine::GetInstance().GenerateRandom<float>(min, max) * (totalGoalsScored / 70.0f);
                    
                    // Adjust overall rating increase bounds based on the current training staff level hired
                    if (generatedWeight >= (2200.0f - (staffLevel * 400.0f)) && generatedWeight < (2900.0f - (staffLevel * 450.0f)))
                        overallIncreaseAmount = 1;
                    else if (generatedWeight >= (2900.0f - (staffLevel * 450)))
                        overallIncreaseAmount = 2;
                }
                else
                {
                    // THIS IS FOR DEFENSIVE MINDED PLAYERS e.g. CDM, CB, LB, GK etc
                    const float min = 1000.0f / ((float)std::max(totalGoalsConceded, 1) / 100.0f);
                    const float max = 1650.0f / ((float)std::max(totalGoalsConceded, 1) / 100.0f);
                    const float generatedWeight = RandomEngine::GetInstance().GenerateRandom<float>(min, max);

                    // Adjust overall rating increase bounds based on the current training staff level hired
                    if (generatedWeight >= (3500.0f - (staffLevel * 850.0f)) && generatedWeight < (4500.0f - (staffLevel * 850.0f)))
                        overallIncreaseAmount = 1;
                    else if (generatedWeight >= (4500.0f - (staffLevel * 850.0f)))
                        overallIncreaseAmount = 2;
                }

                // Young players (under 20) which are below 65 rated have a chance of getting a bonus overall rating increase
                // Note that this only applies if coaches for the player's position have been hired for the season
                const int generatedBonusWeight = RandomEngine::GetInstance().GenerateRandom<int>(0, 1000);
                if (player->GetAge() < 20 && player->GetOverall() <= 65)
                {
                    if ((staffLevel == 1 && generatedBonusWeight >= 700) || (staffLevel == 2 && generatedBonusWeight >= 500) ||
                        (staffLevel == 3 && generatedBonusWeight >= 300) || (staffLevel == 4 && generatedBonusWeight >= 100))
                    {
                        overallIncreaseAmount += (int)(staffLevel > 1) + 1;
                    }
                }

                if (overallIncreaseAmount > 0)
                {
                    // Increase their value based on amount of growth
                    const int valueIncrease = RandomEngine::GetInstance().GenerateRandom<int>(250000, 1000000) * 
                        std::max((int)(overallIncreaseAmount + ((float)((player->GetPotential() - std::max(player->GetOverall(), 70)) / 10.0f))), 1);

                    player->SetValue(Util::GetTruncatedSFInteger(player->GetValue() + valueIncrease, 4));

                    // Add the generated overall increase amount onto the player's current overall
                    player->SetOverall(player->GetOverall() + overallIncreaseAmount);
                    this->improvedPlayers[player->GetID()] = overallIncreaseAmount;
                }
            }
        }
    }

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "NEXT"));
    
    this->userInterface.AddSelectionList("Improved Players", SelectionList({ 960, 490 }, { 1860, 720 }, 80));
    this->userInterface.GetSelectionList("Improved Players")->AddCategory("Player Name");

    if (SaveData::GetInstance().GetGrowthSystemType() == SaveData::GrowthSystemType::OVERALL_RATING)
    {
        this->userInterface.GetSelectionList("Improved Players")->AddCategory("Previous Overall Rating");
        this->userInterface.GetSelectionList("Improved Players")->AddCategory("Current Overall Rating");
        this->userInterface.GetSelectionList("Improved Players")->AddCategory("Overall Increase");
    }
    else
    {
        this->userInterface.GetSelectionList("Improved Players")->AddCategory("Available Skill Points");
    }

    this->SetupImprovedPlayersList();
}

void PlayerGrowthGeneration::Destroy() {}

void PlayerGrowthGeneration::SetupImprovedPlayersList()
{
    // Clear the selection list first
    this->userInterface.GetSelectionList("Improved Players")->Clear();

    // Loop through the players in the user's club
    for (const Player* player : SaveData::GetInstance().GetUsers()[this->userIndex].GetClub()->GetPlayers())
    {
        if (this->improvedPlayers.find(player->GetID()) != this->improvedPlayers.end())
        {
            if (SaveData::GetInstance().GetGrowthSystemType() == SaveData::GrowthSystemType::OVERALL_RATING)
            {
                this->userInterface.GetSelectionList("Improved Players")->AddElement({ std::string(player->GetName()),
                    std::to_string(player->GetOverall() - this->improvedPlayers[player->GetID()]), std::to_string(player->GetOverall()),
                    "+" + std::to_string(this->improvedPlayers[player->GetID()])}, -1);
            }
            else
            {
                std::string skillPointsAmountString = "+" + std::to_string(this->improvedPlayers[player->GetID()] * 15);
                this->userInterface.GetSelectionList("Improved Players")->AddElement({ std::string(player->GetName()), skillPointsAmountString }, -1);
            }
        }
    }
}

void PlayerGrowthGeneration::Update(const float& deltaTime)
{
    // Update the user interface
    this->userInterface.Update(deltaTime);

    // Check if any of othe buttons has been clicked
    for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
    {
        const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
        if (button->GetText() == "NEXT" && button->WasClicked())
        {
            if ((this->userIndex + 1) == (int)SaveData::GetInstance().GetUsers().size())
            {
                this->PushState(NewSeasonSetup::GetAppState());
            }
            else
            {
                ++this->userIndex;
                this->SetupImprovedPlayersList();
            }
        }
    }
}

void PlayerGrowthGeneration::Render() const
{
    const Club* userClub = SaveData::GetInstance().GetUsers()[this->userIndex].GetClub();

    // Render the app state title
    const glm::vec2 titleTextSize = Renderer::GetInstance().GetTextSize(this->font, 75, std::string(userClub->GetName()) + " SQUAD GROWTH");
    Renderer::GetInstance().RenderShadowedText({ 1940 - titleTextSize.x, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        std::string(userClub->GetName()) + " SQUAD GROWTH", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool PlayerGrowthGeneration::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool PlayerGrowthGeneration::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

PlayerGrowthGeneration* PlayerGrowthGeneration::GetAppState()
{
    static PlayerGrowthGeneration appState;
    return &appState;
}
