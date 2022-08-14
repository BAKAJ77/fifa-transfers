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
        UserFinancials calculatedFinancials;
        calculatedFinancials.previousTransferBudget = user.GetClub()->GetTransferBudget();
        calculatedFinancials.previousWageBudget = user.GetClub()->GetWageBudget();

        // First calculate the total wages to be paid to all the players in the user's club
        for (Player* player : user.GetClub()->GetPlayers())
            calculatedFinancials.totalWages += (player->GetWage() * 51);

        int gamesWon = 0, gamesDrawn = 0, gamesLost = 0;
        int totalObjectivesIncomplete = 0;
        float totalRevenueBonus = 1.0f;

        for (const UserProfile::CompetitionData& compStats : user.GetCompetitionData())
        {
            // Tally up the total games won, drawn and lost
            gamesWon += compStats.currentWins;
            gamesDrawn += compStats.currentDraws;
            gamesLost += compStats.currentLosses;

            // Add the revenue bonuses from the competitions won by the user
            if (compStats.compID > 1000) // DOMESTIC CUP COMPETITION
            {
                if (compStats.seasonEndPosition == SaveData::GetInstance().GetCup(compStats.compID)->GetRounds().size() + 1) // The user won the cup?
                    totalRevenueBonus += SaveData::GetInstance().GetCup(compStats.compID)->GetWinnerBonus();
            }
            else // LEAGUE COMPETITION
            {
                if (compStats.seasonEndPosition == 1)
                    totalRevenueBonus += SaveData::GetInstance().GetLeague(compStats.compID)->GetTitleBonus();
                else if (compStats.wonPlayoffs || compStats.seasonEndPosition <= SaveData::GetInstance().GetLeague(compStats.compID)->GetAutoPromotionThreshold())
                    totalRevenueBonus += (SaveData::GetInstance().GetLeague(compStats.compID)->GetTitleBonus() / 2.0f);
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
                        totalRevenueBonus += 0.5f;
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
        float totalClubWages = 0;
        for (const Player* player : user.GetClub()->GetPlayers())
            totalClubWages += player->GetWage();

        calculatedFinancials.newWageBudget = std::max(Util::GetTruncatedSFInteger((int)(totalClubWages / (4.03306f + ((float)totalObjectivesIncomplete))), 3), 
            calculatedFinancials.previousWageBudget);

        // Calculate the total revenue made by the club
        const float generatedRevenueMultiplier = ((gamesWon / 3.0f) + (gamesDrawn / 12.0f) + (user.GetClub()->GetAverageOverall() / 45.0f)) * totalRevenueBonus;
        calculatedFinancials.totalRevenue = (int)((float)RandomEngine::GetInstance().GenerateRandom<int>(1000000, 3500000) * (generatedRevenueMultiplier / 10.0f));
        calculatedFinancials.totalRevenue += calculatedFinancials.totalWages;

        calculatedFinancials.totalRevenue = Util::GetTruncatedSFInteger(calculatedFinancials.totalRevenue, 4);

        // Calculate the club's total expenses
        const float generatedExpensesMultiplier = ((gamesLost / 7.0f) + (user.GetClub()->GetAverageOverall() / 45.0f));
        calculatedFinancials.totalExpenses = (int)((float)RandomEngine::GetInstance().GenerateRandom<int>(100000, 1500000) * generatedExpensesMultiplier);
        calculatedFinancials.totalExpenses += calculatedFinancials.totalWages;

        calculatedFinancials.totalExpenses = Util::GetTruncatedSFInteger(calculatedFinancials.totalExpenses, 4);

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

            user.GetClub()->SetTransferBudget(std::max(user.GetClub()->GetTransferBudget() + (userFinancials.totalRevenue - userFinancials.totalExpenses), 0));
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

    Renderer::GetInstance().RenderShadowedText({ 60, 220 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 60, "CLUB CASH FLOW", 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 280 }, { 0, 255, 0, this->userInterface.GetOpacity() }, this->font, 35,
        "- Total Revenue: " + Util::GetFormattedCashString(userFinancials.totalRevenue), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 340 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 35,
        "- Total Wages Paid: " + Util::GetFormattedCashString(userFinancials.totalWages), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 400 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 35,
        "- Total Expenses: " + Util::GetFormattedCashString(userFinancials.totalExpenses), 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 500 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 60, "NEXT SEASON'S BUDGETS", 5);

    Renderer::GetInstance().RenderShadowedText({ 60, 560 }, { glm::vec3(255), this->userInterface.GetOpacity()}, this->font, 35,
        "- Previous Wage Budget: " + Util::GetFormattedCashString(userFinancials.previousWageBudget), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 620 }, { 0, 200, 200, this->userInterface.GetOpacity() }, this->font, 35,
        "- New Wage Budget: " + Util::GetFormattedCashString(userFinancials.newWageBudget), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 680 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
        "- Previous Transfer Budget: " + Util::GetFormattedCashString(userFinancials.previousTransferBudget), 5);
    Renderer::GetInstance().RenderShadowedText({ 60, 740 }, { 0, 200, 200, this->userInterface.GetOpacity() }, this->font, 35,
        "- New Transfer Budget: " + 
        Util::GetFormattedCashString(std::max(userFinancials.previousTransferBudget + (userFinancials.totalRevenue - userFinancials.totalExpenses), 0)), 5);

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
