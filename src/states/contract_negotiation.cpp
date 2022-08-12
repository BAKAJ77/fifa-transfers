#include <states/contract_negotiation.h>
#include <states/contract_response.h>
#include <states/view_player.h>
#include <states/main_game.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/random_engine.h>
#include <util/data_manip.h>
#include <util/globals.h>

void ContractNegotiation::Init()
{
    // Initialize the member variables
    this->exitState = this->wentBack = this->onNegotiationCooldown = this->leagueTierInsufficient = this->lengthInvalid = this->wageInvalid = 
        this->releaseClauseInvalid = this->sellerSquadTooSmall = this->buyerSquadTooLarge = false;
    
    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Make sure the buying user's squad isn't at the maximum limit and that the selling team is not at the minimum squad limit
    if (SaveData::GetInstance().GetClub(this->negotiatingPlayer->GetClub())->GetPlayers().size() <= Globals::minSquadSize)
        this->sellerSquadTooSmall = true;
    else if (MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetPlayers().size() >= Globals::maxSquadSize)
        this->buyerSquadTooLarge = true;

    // For more realism, players who play in way higher leagues tiers shouldn't be as interested in joining.
    const League* playerLeague = SaveData::GetInstance().GetLeague(SaveData::GetInstance().GetClub(this->negotiatingPlayer->GetClub())->GetLeague());
    const League* userLeague = SaveData::GetInstance().GetCurrentLeague();

    if (playerLeague->GetTier() <= userLeague->GetTier() - 2)
        this->leagueTierInsufficient = true;

    // Check if there is an active negotiation cooldown with the user's club and the player
    if (!this->leagueTierInsufficient)
    {
        for (const SaveData::NegotiationCooldown& cooldown : SaveData::GetInstance().GetNegotiationCooldowns())
        {
            if ((cooldown.clubID == MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID() || cooldown.clubID == 0) &&
                cooldown.playerID == this->negotiatingPlayer->GetID() && cooldown.type == SaveData::CooldownType::CONTRACT_NEGOTIATING)
            {
                this->onNegotiationCooldown = true;
                break;
            }
        }
    }

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    if (!this->onNegotiationCooldown && !this->leagueTierInsufficient && !this->sellerSquadTooSmall && !this->buyerSquadTooLarge)
    {
        this->userInterface.AddButton(new MenuButton({ 1745, 880 }, { 300, 100 }, { 315, 115 }, "SUBMIT"));

        this->userInterface.AddDropDown("Amount Of Years", DropDown({ 210, 540 }, { 300, 75 }, 255, 35, 0));
        this->userInterface.GetDropDown("Amount Of Years")->AddSelection("1", 1);
        this->userInterface.GetDropDown("Amount Of Years")->AddSelection("2", 2);
        this->userInterface.GetDropDown("Amount Of Years")->AddSelection("3", 3);
        this->userInterface.GetDropDown("Amount Of Years")->AddSelection("4", 4);
        this->userInterface.GetDropDown("Amount Of Years")->AddSelection("5", 5);

        this->userInterface.AddTextField("Wage", TextInputField({ 210, 710 }, { 300, 75 },
            TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));

        this->userInterface.AddTickBox("Release Clause Included", TickBox({ 60, 830 }, { 40, 40 }, "Include release clause in the contract?", 255, 0));
        this->userInterface.AddTextField("Release Clause", TextInputField({ 210, 970 }, { 300, 75 },
            TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES, 0));
    }
}

void ContractNegotiation::Destroy() {}

bool ContractNegotiation::ValidateInputs()
{
    this->lengthInvalid = this->userInterface.GetDropDown("Amount Of Years")->GetCurrentSelected() == -1;
    
    if (this->userInterface.GetTextField("Wage")->GetInputtedText().empty() || std::stoi(this->userInterface.GetTextField("Wage")->GetInputtedText()) <= 0 ||
        (std::stoi(this->userInterface.GetTextField("Wage")->GetInputtedText()) - this->negotiatingPlayer->GetWage()) > 
        MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetWageBudget())
    {
        this->wageInvalid = true;
    }
    else
        this->wageInvalid = false;

    if (this->userInterface.GetTickBox("Release Clause Included")->isCurrentlyTicked())
    {
        if (this->userInterface.GetTextField("Release Clause")->GetInputtedText().empty() ||
            std::stoi(this->userInterface.GetTextField("Release Clause")->GetInputtedText()) <= 0)
        {
            this->releaseClauseInvalid = true;
        }
        else
            this->releaseClauseInvalid = false;
    }
    else
        this->releaseClauseInvalid = false;

    return !this->lengthInvalid && !this->wageInvalid && !this->releaseClauseInvalid;
}

void ContractNegotiation::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of other buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "SUBMIT" && button->WasClicked() && this->ValidateInputs())
            {
                int releaseClause = 0;
                if (this->userInterface.GetTickBox("Release Clause Included")->isCurrentlyTicked())
                    releaseClause = std::stoi(this->userInterface.GetTextField("Release Clause")->GetInputtedText());

                ContractResponse::GetAppState()->SetContractOffer(this->negotiatingPlayer, this->userInterface.GetDropDown("Amount Of Years")->GetCurrentSelected(),
                    std::stoi(this->userInterface.GetTextField("Wage")->GetInputtedText()), releaseClause, this->renewingContract, this->callerAppState,
                    this->finishedNegotiating);

                this->PushState(ContractResponse::GetAppState());
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
                this->exitState = this->wentBack = true;
        }

        if (!this->onNegotiationCooldown && !this->leagueTierInsufficient && !this->sellerSquadTooSmall && !this->buyerSquadTooLarge)
        {
            // Only show the release clause text input box if a release clause is to be included
            if (this->userInterface.GetTickBox("Release Clause Included")->isCurrentlyTicked())
                this->userInterface.GetTextField("Release Clause")->SetOpacity(255);
            else
                this->userInterface.GetTextField("Release Clause")->SetOpacity(0);
        }
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->RollBack(this->callerAppState);
    }
}

void ContractNegotiation::Render() const
{
    // Render the state background
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderShadowedText({ 1040, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "CONTRACT NEGOTIATION", 5);

    if (!this->onNegotiationCooldown && !this->leagueTierInsufficient && !this->sellerSquadTooSmall && !this->buyerSquadTooLarge)
    {
        // Render text displaying the club's remaining wage budget and the current wage and release clause of the player
        Renderer::GetInstance().RenderShadowedText({ 60, 220 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 70,
            std::string("REMAINING CLUB WAGE BUDGET: ") + Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetWageBudget()), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 300 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 70,
            std::string("CURRENT WAGE: ") + Util::GetFormattedCashString(this->negotiatingPlayer->GetWage()), 5);

        if (this->negotiatingPlayer->GetReleaseClause() > 0)
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 380 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 70,
                std::string("CURRENT RELEASE CLAUSE: ") + Util::GetFormattedCashString(this->negotiatingPlayer->GetReleaseClause()), 5);
        }
        else
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 380 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 70,
                "CURRENT RELEASE CLAUSE: N/A", 5);
        }

        // Render the text labelling the contract negotiation options to configure
        if (this->renewingContract)
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 480 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
                "Select the number of years you want to extend the contract by", 5);
        }
        else
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 480 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
                "Select the contract length you want to offer", 5);
        }

        Renderer::GetInstance().RenderShadowedText({ 60, 650 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
            "Enter the wage amount you want to offer:", 5);

        if (this->userInterface.GetTextField("Release Clause")->GetOpacity() == 255)
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 910 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
                "Enter the release clause price you want to include:", 5);
        }

        // If any input validations fail, render error asterisk next to the input element
        if (this->lengthInvalid)
            Renderer::GetInstance().RenderText({ 380, 555 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

        if (this->wageInvalid)
            Renderer::GetInstance().RenderText({ 380, 725 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

        if (this->releaseClauseInvalid)
            Renderer::GetInstance().RenderText({ 380, 985 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");
    }
    else if (this->sellerSquadTooSmall)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
            std::string(SaveData::GetInstance().GetClub(this->negotiatingPlayer->GetClub())->GetName()) +
            " are unable to sell since they only have 16 players in their squad.", 5);
    }
    else if (this->buyerSquadTooLarge)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
            "At the moment, you are unable to buy anyone since you are at the max squad size limit of 52 players.", 5);
    }
    else if (this->leagueTierInsufficient)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 30,
            std::string(this->negotiatingPlayer->GetName().data()) + " doesn't want to play in a league which isn't competitive enough for his skills.", 5);
    }
    else if (this->onNegotiationCooldown)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 200 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            std::string(this->negotiatingPlayer->GetName().data()) + "'s camp isn't interested in negotiating with you at the moment.", 5);
    }

    // Render the user interface
    this->userInterface.Render();
}

bool ContractNegotiation::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool ContractNegotiation::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade out effect of the user interface
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool ContractNegotiation::OnResumeTransitionUpdate(const float deltaTime)
{
    return this->OnStartupTransitionUpdate(deltaTime);
}

ContractNegotiation* ContractNegotiation::GetAppState()
{
    static ContractNegotiation appState;
    return &appState;
}

void ContractNegotiation::SetNegotiatingPlayer(Player* player, AppState* callerAppState, bool renewingContract, bool* finishedNegotiating)
{
    this->negotiatingPlayer = player;
    this->renewingContract = renewingContract;
    this->callerAppState = callerAppState;

    this->finishedNegotiating = finishedNegotiating;
}

bool ContractNegotiation::WentBack() const
{
    return this->wentBack;
}
