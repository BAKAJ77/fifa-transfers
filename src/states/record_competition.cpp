#include <states/record_competition.h>
#include <states/end_competition.h>
#include <states/main_game.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>

void RecordCompetition::Init()
{
    // Initialize the member variables
    this->exitState = this->completed = this->goalsScoredInvalid = this->goalsConcededInvalid = this->gamesWonInvalid = this->gamesDrawnInvalid =
        this->gamesLostInvalid = this->roundsInvalid = this->wonCupInvalid = this->tablePositionInvalid = false;

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

bool RecordCompetition::ValidateInputs()
{
    // Make sure the basic inputs are not empty
    this->goalsScoredInvalid = this->userInterface.GetTextField("Goals Scored")->GetInputtedText().empty();
    this->goalsConcededInvalid = this->userInterface.GetTextField("Goals Conceded")->GetInputtedText().empty();
    this->gamesWonInvalid = this->userInterface.GetTextField("Games Won")->GetInputtedText().empty();
    this->gamesDrawnInvalid = this->userInterface.GetTextField("Games Drawn")->GetInputtedText().empty();
    this->gamesLostInvalid = this->userInterface.GetTextField("Games Lost")->GetInputtedText().empty();

    if (this->selectedCup)
    {
        // Make sure a value has been selected in the rounds drop down and that the amount of users selecting a specific rounds
        // is limited to make sense e.g. only 2 users max can be in the final.
        const int selectedRound = this->userInterface.GetDropDown("Rounds")->GetCurrentSelected();
        if (selectedRound != -1)
        {
            int roundTakenCount = 0;
            for (const CompetitionStats& stats : this->recordedCompetitionStats)
            {
                if (stats.seasonEndPosition == selectedRound)
                    ++roundTakenCount;
            }

            if (selectedRound == this->selectedCup->GetRounds().size() && roundTakenCount == 2) // FINAL ROUND
                this->roundsInvalid = true;
            else if (selectedRound == (this->selectedCup->GetRounds().size() - 1) && roundTakenCount == 4) // SEMI FINAL ROUND
                this->roundsInvalid = true;
            else
            {
                this->roundsInvalid = false;

                // If two users selected that they made it to the final, make sure that one has selected that they won the cup and 
                // other has selected that they did not
                if (selectedRound == this->selectedCup->GetRounds().size())
                {
                    bool otherUserWonCup = false, anotherUserFinalist = false;
                    for (const CompetitionStats& stats : this->recordedCompetitionStats)
                    {
                        if (stats.seasonEndPosition == selectedRound)
                        {
                            otherUserWonCup = stats.wonCup;
                            anotherUserFinalist = true;
                            break;
                        }
                    }

                    if (this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected() == -1)
                    {
                        this->wonCupInvalid = true;
                    }
                    else if (anotherUserFinalist && (otherUserWonCup && (bool)this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected()) ||
                        (!otherUserWonCup && !(bool)this->userInterface.GetRadioButtonGroup("Won Cup")->GetSelected()))
                    {
                        this->wonCupInvalid = true;
                    }
                    else
                        this->wonCupInvalid = false;
                }
            }
        }
        else
            this->roundsInvalid = true;
    }
    else
    {
        // Make sure the table position input is not empty, is within valid bounds according to the amount of teams in the league and
        // is not already taken by another user.
        if (!this->userInterface.GetTextField("Table Position")->GetInputtedText().empty())
        {
            const uint16_t tablePosition = (uint16_t)std::stoi(this->userInterface.GetTextField("Table Position")->GetInputtedText());

            if (tablePosition < 1 || tablePosition > SaveData::GetInstance().GetCurrentLeague()->GetClubs().size())
                this->tablePositionInvalid = true;
            else
            {
                bool tablePositionTaken = false;
                for (const CompetitionStats& stats : this->recordedCompetitionStats)
                {
                    if (stats.seasonEndPosition == tablePosition)
                    {
                        tablePositionTaken = true;
                        break;
                    }
                }

                this->tablePositionInvalid = tablePositionTaken;
            }
        }
        else
            this->tablePositionInvalid = true;
    }

    return !this->goalsScoredInvalid && !this->goalsConcededInvalid && !this->gamesWonInvalid && !this->gamesDrawnInvalid && !this->gamesLostInvalid &&
        !this->roundsInvalid && !this->wonCupInvalid && !this->tablePositionInvalid;
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
                if (this->ValidateInputs())
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

                        // Clear the user interface inputs
                        this->userInterface.GetTextField("Goals Scored")->Clear();
                        this->userInterface.GetTextField("Goals Conceded")->Clear();
                        this->userInterface.GetTextField("Games Won")->Clear();
                        this->userInterface.GetTextField("Games Drawn")->Clear();
                        this->userInterface.GetTextField("Games Lost")->Clear();

                        if (this->selectedCup)
                        {
                            this->userInterface.GetDropDown("Rounds")->Reset();
                            this->userInterface.GetRadioButtonGroup("Won Cup")->Reset();
                        }
                        else
                            this->userInterface.GetTextField("Table Position")->Clear();
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

    // Render any input validation errors that occur
    if (this->goalsScoredInvalid)
        Renderer::GetInstance().RenderText({ 660, 260 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->goalsConcededInvalid)
        Renderer::GetInstance().RenderText({ 660, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->gamesWonInvalid)
        Renderer::GetInstance().RenderText({ 660, 700 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->gamesDrawnInvalid)
        Renderer::GetInstance().RenderText({ 660, 920 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->gamesLostInvalid)
        Renderer::GetInstance().RenderText({ 1830, 260 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->selectedCup)
    {
        if (this->roundsInvalid)
            Renderer::GetInstance().RenderText({ 1830, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

        if (this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() != -1 &&
            this->selectedCup->GetRounds()[this->userInterface.GetDropDown("Rounds")->GetCurrentSelected() - 1] == "Final")
        {
            if (this->wonCupInvalid)
                Renderer::GetInstance().RenderText({ 1830, 700 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");
        }
    }
    else
    {
        if (this->tablePositionInvalid)
            Renderer::GetInstance().RenderText({ 1830, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");
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
