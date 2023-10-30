#include <states/financials_generation.h>
#include <states/player_growth_generation.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/random_engine.h>
#include <util/data_manip.h>

void FinancialsGeneration::Init()
{
    // Initialized the member variables
    this->userIndex = 0;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "NEXT"));
    
    // Calculate the finances of each user in the save
    for (UserProfile& user : SaveData::GetInstance().GetUsers())
    {
        const float objectiveBonusAmount = 1.0f + (0.3f / (float)user.GetClub()->GetObjectives().size());

        UserFinancials calculatedFinancials;
        calculatedFinancials.previousTransferBudget = user.GetClub()->GetTransferBudget();
        calculatedFinancials.previousWageBudget = user.GetClub()->GetWageBudget();

        // First calculate the total wages to be paid to all the players in the user's club
        for (Player* player : user.GetClub()->GetPlayers())
            calculatedFinancials.totalWages += (player->GetWage() * 51);

        int gamesWon = 0, gamesDrawn = 0, gamesLost = 0;
        int totalObjectivesIncomplete = 0;

        int totalWinnerBonus = 0;
        float totalObjectiveBonus = 0.0f;

        for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
        {
            // Tally up the total games won, drawn and lost
            gamesWon += compStats.currentWins;
            gamesDrawn += compStats.currentDraws;
            gamesLost += compStats.currentLosses;

            // Add the revenue bonuses from the competitions won by the user
            if (compStats.compID >= 1000) // DOMESTIC CUP COMPETITION
            {
                if (compStats.seasonEndPosition == SaveData::GetInstance().GetCup(compStats.compID)->GetRounds().size() + 1) // The user won the cup?
                    totalWinnerBonus += SaveData::GetInstance().GetCup(compStats.compID)->GetWinnerBonus();
                else if (compStats.seasonEndPosition == SaveData::GetInstance().GetCup(compStats.compID)->GetRounds().size()) // The user is runners up?
                    totalWinnerBonus += (int)(SaveData::GetInstance().GetCup(compStats.compID)->GetWinnerBonus() / 2.5f);
            }
            else // LEAGUE COMPETITION
            {
                if (compStats.seasonEndPosition == 1)
                    totalWinnerBonus += SaveData::GetInstance().GetLeague(compStats.compID)->GetTitleBonus();
                else if (compStats.wonPlayoffs || compStats.seasonEndPosition <= SaveData::GetInstance().GetLeague(compStats.compID)->GetAutoPromotionThreshold())
                    totalWinnerBonus += (int)(SaveData::GetInstance().GetLeague(compStats.compID)->GetTitleBonus() / 2.5f);
            }

            // Tally up the total amount of club objectives that the user didn't complete
            // Also add a revenue bonus per objective the user completed
            for (const Club::Objective& objective : user.GetClub()->GetObjectives())
            {
                if (objective.compID == compStats.compID)
                {
                    if ((compStats.compID > 1000 && compStats.seasonEndPosition >= objective.targetEndPosition) ||
                        (compStats.compID < 1000 && compStats.seasonEndPosition <= objective.targetEndPosition))
                    {
                        totalObjectiveBonus += objectiveBonusAmount;
                    }
                    else
                    {
                        ++totalObjectivesIncomplete;
                    }

                    break;
                }
            }
        }

        // Calculate the user's club's new wage budget
        const int previousInitialWageBudget = user.GetClub()->GetInitialWageBudget();
        user.GetClub()->SetInitialWageBudget(
            Util::GetTruncatedSFInteger((int)((float)user.GetClub()->GetInitialWageBudget() * totalObjectiveBonus), 3));

        calculatedFinancials.newWageBudget = user.GetClub()->GetWageBudget() > user.GetClub()->GetInitialWageBudget() ?
            user.GetClub()->GetWageBudget() + (user.GetClub()->GetInitialWageBudget() - previousInitialWageBudget) :
            user.GetClub()->GetInitialWageBudget();

        // Calculate the user's club's new transfer budget
        const int previousInitialTransferBudget = user.GetClub()->GetInitialTransferBudget();
        user.GetClub()->SetInitialTransferBudget(
            Util::GetTruncatedSFInteger((int)((float)user.GetClub()->GetInitialTransferBudget() * totalObjectiveBonus), 4));

        calculatedFinancials.newTransferBudget = user.GetClub()->GetTransferBudget() > user.GetClub()->GetInitialTransferBudget() ?
            user.GetClub()->GetTransferBudget() + totalWinnerBonus + (user.GetClub()->GetInitialTransferBudget() - previousInitialTransferBudget) :
            user.GetClub()->GetInitialTransferBudget() + totalWinnerBonus;
        
        // Calculate the total revenue made by the club
        const float generatedRevenueMultiplier = ((gamesWon / 3.0f) + (gamesDrawn / 12.0f) + (user.GetClub()->GetAverageOverall() / 30.0f) * 
            (totalObjectiveBonus + 1.0f)) + totalWinnerBonus;

        calculatedFinancials.totalRevenue = (int)((float)RandomEngine::GetInstance().GenerateRandom<int>(1000000, 3500000) * (generatedRevenueMultiplier / 10.0f));
        calculatedFinancials.totalRevenue += calculatedFinancials.totalWages;

        calculatedFinancials.totalRevenue = Util::GetTruncatedSFInteger(calculatedFinancials.totalRevenue * 20, 4);

        // Calculate the club's total expenses
        const float generatedExpensesMultiplier = ((gamesLost / 7.0f) + (user.GetClub()->GetAverageOverall() / 45.0f));
        calculatedFinancials.totalExpenses = (int)((float)RandomEngine::GetInstance().GenerateRandom<int>(100000, 1500000) * generatedExpensesMultiplier);
        calculatedFinancials.totalExpenses += calculatedFinancials.totalWages;

        calculatedFinancials.totalExpenses = Util::GetTruncatedSFInteger(calculatedFinancials.totalExpenses * 20, 4);

        // Push the user's calculated financials into the vector
        this->calculatedUserFinancials.emplace_back(calculatedFinancials);
    }
}

void FinancialsGeneration::Destroy() {}

void FinancialsGeneration::Update(const float& deltaTime)
{
    // Update the user interface
    this->userInterface.Update(deltaTime);

    // Check if any of othe buttons has been clicked
    for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
    {
        const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
        if (button->GetText() == "NEXT" && button->WasClicked())
        {
            // Assign the new next season transfer and wage budgets
            const UserFinancials& userFinancials = this->calculatedUserFinancials[this->userIndex];
            UserProfile& user = SaveData::GetInstance().GetUsers()[this->userIndex];

            user.GetClub()->SetTransferBudget(userFinancials.newTransferBudget);
            user.GetClub()->SetWageBudget(userFinancials.newWageBudget);
            
            if ((this->userIndex + 1) == (int)SaveData::GetInstance().GetUsers().size())
            {
                this->PushState(PlayerGrowthGeneration::GetAppState());
            }
            else
            {
                ++this->userIndex;
            }
        }
    }
}

void FinancialsGeneration::Render() const
{
    const UserProfile& user = SaveData::GetInstance().GetUsers()[this->userIndex];
    const UserFinancials& userFinancials = this->calculatedUserFinancials[this->userIndex];

    // Render the background and app state title
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });

    const glm::vec2 titleTextSize = Renderer::GetInstance().GetTextSize(this->font, 75, std::string(user.GetName()) + "'s FINANCIAL OVERVIEW");
    Renderer::GetInstance().RenderShadowedText({ 1960 - titleTextSize.x, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        std::string(user.GetName()) + "'s FINANCIAL OVERVIEW", 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 220 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 80, "CLUB CASH FLOW", 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 310 }, { 0, 255, 0, this->userInterface.GetOpacity() }, this->font, 60,
        "- Total Revenue: " + Util::GetFormattedCashString(userFinancials.totalRevenue), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 390 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 60,
        "- Total Wages Paid: " + Util::GetFormattedCashString(userFinancials.totalWages), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 470 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 60,
        "- Total Expenses: " + Util::GetFormattedCashString(userFinancials.totalExpenses), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 620 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 80, "NEXT SEASON'S BUDGETS", 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 710 }, { glm::vec3(255), this->userInterface.GetOpacity()}, this->font, 60,
        "- Previous Wage Budget: " + Util::GetFormattedCashString(userFinancials.previousWageBudget), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 790 }, { 0, 200, 200, this->userInterface.GetOpacity() }, this->font, 60,
        "- New Wage Budget: " + Util::GetFormattedCashString(userFinancials.newWageBudget), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 870 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 60,
        "- Previous Transfer Budget: " + Util::GetFormattedCashString(userFinancials.previousTransferBudget), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 950 }, { 0, 200, 200, this->userInterface.GetOpacity() }, this->font, 60,
        "- New Transfer Budget: " + Util::GetFormattedCashString(userFinancials.newTransferBudget), 5);

    // Render the user interface
    this->userInterface.Render();
}

bool FinancialsGeneration::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool FinancialsGeneration::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

FinancialsGeneration* FinancialsGeneration::GetAppState()
{
    static FinancialsGeneration appState;
    return &appState;
}
