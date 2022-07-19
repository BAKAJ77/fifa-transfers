#include <states/record_competition.h>
#include <states/end_competition.h>
#include <states/main_game.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>

void RecordCompetition::Init()
{
    // Initialize the member variables
    this->exitState = this->completed = false;
    this->userProfileIndex = 0;
    
    EndCompetition::GetAppState()->GetCompetitionSelectionList().GetCurrentSelected() > 1000 ? // Cup competitions have an ID exceeding 1000
        this->selectedCup = SaveData::GetInstance().GetCup(EndCompetition::GetAppState()->GetCompetitionSelectionList().GetCurrentSelected()) :
        this->selectedCup = nullptr;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddTextField("Goals Scored", TextInputField({ 330, 245 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    this->userInterface.AddTextField("Goals Conceded", TextInputField({ 330, 465 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    
    this->userInterface.AddTextField("Games Won", TextInputField({ 330, 685 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    this->userInterface.AddTextField("Games Drawn", TextInputField({ 330, 905 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    this->userInterface.AddTextField("Games Lost", TextInputField({ 1500, 245 }, { 600, 75 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));

    if (this->selectedCup) 
    {
        this->userInterface.AddDropDown("Rounds", DropDown({ 1500, 465 }, { 600, 75 }));
        for (size_t index = 0; index < this->selectedCup->GetRounds().size(); index++)
            this->userInterface.GetDropDown("Rounds")->AddSelection(this->selectedCup->GetRounds()[index], (int)(index + 1));

        this->userInterface.AddRadioButtonGroup("Won Cup", RadioButtonGroup({ 1225, 665 }, { 50, 50 }, 0.0f));
        this->userInterface.GetRadioButtonGroup("Won Cup")->Add("Yes", 1);
        this->userInterface.GetRadioButtonGroup("Won Cup")->Add("No", 0);
    }
    else
    {
        this->userInterface.AddTextField("Table Position", TextInputField({ 1500, 465 }, { 600, 75 },
            TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));
    }
}

void RecordCompetition::Destroy() 
{
    this->recordedCompetitionStats.clear();
}

void RecordCompetition::Update(const float& deltaTime)
{
    if (!this->exitState && !this->completed)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of the buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "CONFIRM" && button->WasClicked())
            {
                // Store the user's entered competition stats
                CompetitionStats userStats = { std::stoi(this->userInterface.GetTextField("Goals Scored")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Goals Conceded")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Games Won")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Games Drawn")->GetInputtedText()),
                    std::stoi(this->userInterface.GetTextField("Games Lost")->GetInputtedText()) };

                if (this->selectedCup)
                {
                    userStats.seasonEndPosition = this->userInterface.GetDropDown("Rounds")->GetCurrentSelected();
                    if (this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected())
                        userStats.wonCup = true;
                }
                else
                    userStats.seasonEndPosition = std::stoi(this->userInterface.GetTextField("Table Position")->GetInputtedText());

                this->recordedCompetitionStats.emplace_back(userStats);

                if (this->userProfileIndex + 1 < SaveData::GetInstance().GetUsers().size())
                {
                    // Update the user profile iteration index
                    ++this->userProfileIndex;
                }
                else
                {
                    // Update the user profile's competition stats
                    for (size_t index = 0; index < SaveData::GetInstance().GetUsers().size(); index++)
                    {
                        const CompetitionStats& stats = this->recordedCompetitionStats[index];
                        UserProfile* user = &SaveData::GetInstance().GetUsers()[index];

                        for (UserProfile::CompetitionData& compStats : user->GetCompetitionData())
                        {
                            if (compStats.compID == EndCompetition::GetAppState()->GetCompetitionSelectionList().GetCurrentSelected())
                            {
                                compStats.currentScored += stats.scored;
                                compStats.currentConceded += stats.conceded;
                                compStats.currentWins += stats.wins;
                                compStats.currentDraws += stats.draws;
                                compStats.currentLosses += stats.losses;

                                compStats.totalScored += stats.scored;
                                compStats.totalConceded += stats.conceded;
                                compStats.totalWins += stats.wins;
                                compStats.totalDraws += stats.draws;
                                compStats.totalLosses += stats.losses;

                                if (this->selectedCup)
                                {
                                    compStats.seasonEndPosition = stats.seasonEndPosition;
                                    if (stats.wonCup)
                                        compStats.titlesWon++;
                                }
                                else
                                {
                                    compStats.seasonEndPosition = stats.seasonEndPosition;
                                    if (compStats.seasonEndPosition == 1)
                                        compStats.titlesWon++;
                                }

                                break;
                            }
                        }
                    }

                    // Now mark the app state as 'complete' so we can roll back to the 'ContinueGame' app state
                    this->completed = true;
                }
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        // Display the 'Did you win the final' radio buttons if the competition is a CUP and the FINAL round was selected
        if (this->selectedCup && this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() != -1 &&
            this->selectedCup->GetRounds()[this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() - 1] == "Final")
        {
            this->userInterface.GetRadioButtonGroup("Won Cup")->SetOpacity(255.0f);
        }
        else if (this->selectedCup)
            this->userInterface.GetRadioButtonGroup("Won Cup")->SetOpacity(0.0f);
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the fade out effect of the user interface
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));

        if (this->userInterface.GetOpacity() == 0.0f)
        {
            if (this->completed)
                this->SwitchState(MainGame::GetAppState());
            else
            {
                EndCompetition::GetAppState()->GetCompetitionSelectionList().Reset();
                this->PopState();
            }
        }
    }
}

void RecordCompetition::Render() const
{
    // Render the current user, who's entering their stats, name.
    Renderer::GetInstance().RenderShadowedText({ 310, 80 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        SaveData::GetInstance().GetUsers()[this->userProfileIndex].GetName().data() + std::string("'s competition stats"), 5);

    // Render the text descriptions
    Renderer::GetInstance().RenderShadowedText({ 30, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of goals you scored:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of goals you conceded:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of games you won:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 820 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of games you drew:", 5);

    Renderer::GetInstance().RenderShadowedText({ 1200, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of games you lost:", 5);

    if (this->selectedCup)
    {
        Renderer::GetInstance().RenderShadowedText({ 1200, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            "Select the last round you reached", 5);

        if (this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() != -1 && 
            this->selectedCup->GetRounds()[this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() - 1] == "Final")
        {
            Renderer::GetInstance().RenderShadowedText({ 1200, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
                "Did you win the final?", 5);
        }
    }
    else
    {
        Renderer::GetInstance().RenderShadowedText({ 1200, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            "Enter the table position you finished in:", 5);
    }

    // Render the user interface
    this->userInterface.Render();
}

bool RecordCompetition::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

RecordCompetition* RecordCompetition::GetAppState()
{
    static RecordCompetition appState;
    return &appState;
}
