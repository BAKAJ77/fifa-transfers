#include <states/end_competition.h>
#include <states/record_competition.h>
#include <states/main_game.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>

void EndCompetition::Init()
{
    MainGame::GetAppState()->SetUpdateWhilePaused(false);
    
    // Initialize the member variables
    this->exitState = false;
    
    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddSelectionList("Incomplete Competitions", SelectionList({ 635, 520 }, { 1210, 640 }, 80));
    this->userInterface.GetSelectionList("Incomplete Competitions")->AddCategory("Competition Name");
    this->userInterface.GetSelectionList("Incomplete Competitions")->AddCategory("Prestige Tier");

    // Filter out any cup competitions that have already been completed
    for (const League::CompetitionLink& linkedComp : SaveData::GetInstance().GetCurrentLeague()->GetLinkedCompetitions())
    {
        bool alreadyCompletedCup = false;
        for (const UserProfile::CompetitionData& compStats : SaveData::GetInstance().GetUsers().front().GetCompetitionData())
        {
            if (linkedComp.competitionID == compStats.compID && compStats.seasonEndPosition > 0)
            {
                alreadyCompletedCup = true;
                break;
            }
        }

        if (!alreadyCompletedCup)
        {
            const KnockoutCup* cupComp = SaveData::GetInstance().GetCup(linkedComp.competitionID);
            this->userInterface.GetSelectionList("Incomplete Competitions")->AddElement({ cupComp->GetName().data(), std::to_string(cupComp->GetTier()) },
                cupComp->GetID());
        }
    }

    // Filter out the league competition if it has already been completed
    bool alreadyCompletedLeague = false;
    for (const UserProfile::CompetitionData& compStats : SaveData::GetInstance().GetUsers().front().GetCompetitionData())
    {
        if (SaveData::GetInstance().GetCurrentLeague()->GetID() == compStats.compID && compStats.seasonEndPosition > 0)
        {
            alreadyCompletedLeague = true;
            break;
        }
    }

    if (!alreadyCompletedLeague)
    {
        this->userInterface.GetSelectionList("Incomplete Competitions")->AddElement({ SaveData::GetInstance().GetCurrentLeague()->GetName().data(),
        std::to_string(SaveData::GetInstance().GetCurrentLeague()->GetTier()) }, (int)SaveData::GetInstance().GetCurrentLeague()->GetID());
    }
}

void EndCompetition::Destroy() 
{
    MainGame::GetAppState()->SetUpdateWhilePaused(true);
}

void EndCompetition::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of the buttons have been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = true;
        }

        // Once a competition has been selected, move onto the next section (where the competition stats like goals scored is recorded)
        if (this->userInterface.GetSelectionList("Incomplete Competitions")->GetCurrentSelected() != -1)
            this->PushState(RecordCompetition::GetAppState());
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->PopState();
    }
}

void EndCompetition::Render() const
{
    // Render the text descriptions
    Renderer::GetInstance().RenderShadowedText({ 30, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Select the competition you have completed below", 5);

    // If all the competitions have been completed, render a text string letting the user know
    if (this->userInterface.GetSelectionList("Incomplete Competitions")->GetListElements().empty())
    {
        Renderer::GetInstance().RenderShadowedText({ 30, 1050 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 40,
            "You have already completed all the required competitions!", 5);
    }

    // Render the user interface
    this->userInterface.Render();
}

bool EndCompetition::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool EndCompetition::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool EndCompetition::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

EndCompetition* EndCompetition::GetAppState()
{
    static EndCompetition appState;
    return &appState;
}

int EndCompetition::GetAmountOfIncompleteCompetitions() const
{
    int incompleteCompCount = 0;
    for (const UserProfile::CompetitionData& compStats : SaveData::GetInstance().GetUsers().front().GetCompetitionData())
    {
        if (compStats.compID == SaveData::GetInstance().GetCurrentLeague()->GetID() && compStats.seasonEndPosition == 0)
        {
            ++incompleteCompCount;
        }
        else
        {
            for (const League::CompetitionLink& linkedComp : SaveData::GetInstance().GetCurrentLeague()->GetLinkedCompetitions())
            {
                if (compStats.compID == linkedComp.competitionID && compStats.seasonEndPosition == 0)
                {
                    ++incompleteCompCount;
                }
            }
        }
    }

    return incompleteCompCount;
}

SelectionList& EndCompetition::GetCompetitionSelectionList() const
{
    return *this->userInterface.GetSelectionList("Incomplete Competitions");
}
