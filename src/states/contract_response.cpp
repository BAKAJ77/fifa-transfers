#include <states/contract_response.h>
#include <states/manage_squad.h>
#include <states/view_player.h>
#include <states/main_game.h>

#include <serialization/save_data.h>
#include <interface/menu_button.h>
#include <util/random_engine.h>
#include <util/data_manip.h>

void ContractResponse::Init()
{
    // Initialize the member variables
    this->exitState = this->contractCountered = this->contractRejected = this->negotiationSuccessful = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Execute the contract response generation operation
    const int contractLengthRequest = this->GenerateContractLengthResponse();
    const int wageRequest = this->GenerateWageResponse();

    if (this->contractWage < wageRequest / 2) // A way too low wage offer may offend the player, so outright reject the contract to exit negotiations
        this->contractRejected = true;

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    if (!this->contractRejected && (contractLengthRequest != -1 || wageRequest != -1))
    {
        this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "REJECT"));
        this->userInterface.AddButton(new MenuButton({ 1745, 880 }, { 300, 100 }, { 315, 115 }, "ACCEPT"));

        if (contractLengthRequest != -1)
            this->contractLength = contractLengthRequest;

        if (wageRequest != -1)
            this->contractWage = wageRequest;

        this->contractCountered = true;
    }
    else
        this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "CONTINUE"));
}

void ContractResponse::Destroy() 
{
    if (this->negotiationSuccessful)
    {
        // Apply the new contract details onto the player
        this->renewingContract ?
            this->negotiatingPlayer->SetExpiryYear(this->negotiatingPlayer->GetExpiryYear() + this->contractLength) :
            this->negotiatingPlayer->SetExpiryYear(SaveData::GetInstance().GetCurrentYear() + this->contractLength);

        this->negotiatingPlayer->SetWage(this->contractWage);
        this->negotiatingPlayer->SetReleaseClause(this->contractReleaseClause);

        // If this contract deal is for a player in a different team, then move him to the current user's team
        if (!this->renewingContract)
        {
            SaveData::GetInstance().GetClub(this->negotiatingPlayer->GetClub())->RemovePlayer(this->negotiatingPlayer);
            MainGame::GetAppState()->GetCurrentUser()->GetClub()->AddPlayer(this->negotiatingPlayer);
        }

        // The squad currently loaded into the manage squad list is outdated now, so reload it
        if (this->renewingContract)
            ManageSquad::GetAppState()->ReloadSquad();

        // Push negotiation cooldown with the player for all clubs
        const int cooldownTicks = RandomEngine::GetInstance().GenerateRandom<int>(3, 7);
        SaveData::GetInstance().GetNegotiationCooldowns().push_back({ this->negotiatingPlayer->GetID(), 0, SaveData::CooldownType::CONTRACT_NEGOTIATING,
            cooldownTicks });
    }
    else
    {
        // Push negotiation cooldown with the player for the current user's club only
        const int cooldownTicks = RandomEngine::GetInstance().GenerateRandom<int>(3, 7);
        SaveData::GetInstance().GetNegotiationCooldowns().push_back({ this->negotiatingPlayer->GetID(), 
            MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetID(), SaveData::CooldownType::CONTRACT_NEGOTIATING, cooldownTicks });
    }
}

int ContractResponse::GenerateContractLengthResponse() const
{
	int requestedContractLength = -1; // If this remains as -1, then that means that they accepted it

    // Do small calculations to determine whether the player accepts the contract length given
    const int generatedMultiplier = RandomEngine::GetInstance().GenerateRandom<int>(100, 200);
	int preferenceWeight = 0;

	if (this->renewingContract)
	{
		const int contractYearsRemaining = this->negotiatingPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear();
        preferenceWeight = (int)(((10 - (contractYearsRemaining + this->contractLength)) * generatedMultiplier) / (this->negotiatingPlayer->GetAge() / 20.0f));
	}
	else
        preferenceWeight = (int)(((10 - this->contractLength) * generatedMultiplier) / (this->negotiatingPlayer->GetAge() / 20.0f));

	if (preferenceWeight <= 300)
	{
        // Generate a shorter contract length
        const int lengthOffset = RandomEngine::GetInstance().GenerateRandom<int>(1, 2);
        requestedContractLength = std::max(this->contractLength - lengthOffset, (int)(!this->renewingContract));
	}
    else if (preferenceWeight >= 600)
    {
        // Generate a longer contract length
        const int lengthOffset = RandomEngine::GetInstance().GenerateRandom<int>(1, 2);
        requestedContractLength = std::min(this->contractLength + lengthOffset, 5);
    }

    return requestedContractLength;
}

int ContractResponse::GenerateWageResponse() const
{
    int requestedWages = -1; // If this remains as -1, then that means that they accepted it

    // Calculate the bounds of which the player could request a wage amount from
    const int min = this->negotiatingPlayer->GetWage();
    const int max = (int)(this->negotiatingPlayer->GetWage() * 2.25f);
    
    // Generate the minimum amount the player is willing to accept
    const int minAcceptableWage = Util::GetTruncatedSFInteger(RandomEngine::GetInstance().GenerateRandom<int>(min, max), 3);

    if (this->contractWage < minAcceptableWage)
        requestedWages = minAcceptableWage;

    return requestedWages;
}

void ContractResponse::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of other buttons has been clicked
        for (size_t index = 0; index < this->userInterface.GetButtons().size(); index++)
        {
            const MenuButton* button = (MenuButton*)this->userInterface.GetButtons()[index];
            if (button->GetText() == "CONTINUE" && button->WasClicked())
            {
                this->negotiationSuccessful = !this->contractRejected;
                this->exitState = true;
            }
            else if (button->GetText() == "ACCEPT" && button->WasClicked())
            {
                this->negotiationSuccessful = true;
                this->exitState = true;
            }
            else if (button->GetText() == "REJECT" && button->WasClicked())
            {
                this->negotiationSuccessful = false;
                this->exitState = true;
            }
        }
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the fade out effect of the user interface
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        if (this->userInterface.GetOpacity() == 0.0f)
            this->RollBack(ViewPlayer::GetAppState());
    }
}

void ContractResponse::Render() const
{
    // Render the state background
    Renderer::GetInstance().RenderSquare({ 800, 592.5f }, { 1540, 925 }, { glm::vec3(30), this->userInterface.GetOpacity() });
    Renderer::GetInstance().RenderShadowedText({ 1040, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "CONTRACT NEGOTIATION", 5);

    // Render text describing the player's response to the contract offer
    this->RenderOfferResponse();

    // Render the user interface
    this->userInterface.Render();
}

void ContractResponse::RenderOfferResponse() const
{
    if (!this->contractCountered && !this->contractRejected)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 195 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            std::string(this->negotiatingPlayer->GetName().data()) + " is happy with these terms and has signed the new deal!", 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 325 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 80,
            "NEW CONTRACT DETAILS", 5);
    }
    else if (this->contractCountered)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 195 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            std::string(this->negotiatingPlayer->GetName().data()) + " isn't fully onboard with the offer given and has suggested this instead:", 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 325 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 80,
            "SUGGESTED CONTRACT DETAILS", 5);
    }
    else if (this->contractRejected)
    {
        Renderer::GetInstance().RenderShadowedText({ 60, 195 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 35,
            std::string(this->negotiatingPlayer->GetName().data()) + " is not happy with the contract deal offered and doesn't want to negotiate further.", 5);
    }

    if (!this->contractRejected)
    {
        // Display the new/suggested contract details
        if (this->renewingContract)
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 395 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
                "- YEAR OF EXPIRY: " + std::to_string(this->negotiatingPlayer->GetExpiryYear() + this->contractLength) + " (" +
                std::to_string((this->negotiatingPlayer->GetExpiryYear() + this->contractLength) - SaveData::GetInstance().GetCurrentYear()) + " YEARS)", 5);
        }
        else
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 395 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
                "- YEAR OF EXPIRY: " + std::to_string(SaveData::GetInstance().GetCurrentYear() + this->contractLength) + " (" + std::to_string(this->contractLength) +
                " YEARS)", 5);
        }

        Renderer::GetInstance().RenderShadowedText({ 60, 465 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- WEEKLY WAGE: " +
            Util::GetFormattedCashString(this->contractWage), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 535 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- ANNUAL SALARY: " +
            Util::GetFormattedCashString(this->contractWage * 51), 5);

        if (this->contractReleaseClause > 0)
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- RELEASE CLAUSE: " +
                Util::GetFormattedCashString(this->contractReleaseClause), 5);
        }
        else
            Renderer::GetInstance().RenderShadowedText({ 60, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- RELEASE CLAUSE: N/A", 5);

        // Display the old contract details
        Renderer::GetInstance().RenderShadowedText({ 60, 730 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 80,
            "PREVIOUS CONTRACT DETAILS", 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 800 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
            "- YEAR OF EXPIRY: " + std::to_string(this->negotiatingPlayer->GetExpiryYear()) + " (" +
            std::to_string(this->negotiatingPlayer->GetExpiryYear() - SaveData::GetInstance().GetCurrentYear()) + " YEARS)", 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 870 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- WEEKLY WAGE: " +
            Util::GetFormattedCashString(this->negotiatingPlayer->GetWage()), 5);

        Renderer::GetInstance().RenderShadowedText({ 60, 940 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- ANNUAL SALARY: " +
            Util::GetFormattedCashString(this->negotiatingPlayer->GetWage() * 51), 5);

        if (this->negotiatingPlayer->GetReleaseClause() > 0)
        {
            Renderer::GetInstance().RenderShadowedText({ 60, 1010 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- RELEASE CLAUSE: " +
                Util::GetFormattedCashString(this->negotiatingPlayer->GetReleaseClause()), 5);
        }
        else
            Renderer::GetInstance().RenderShadowedText({ 60, 1010 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50, "- RELEASE CLAUSE: N/A", 5);
    }
}

bool ContractResponse::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

ContractResponse* ContractResponse::GetAppState()
{
    static ContractResponse appState;
    return &appState;
}

void ContractResponse::SetContractOffer(Player* player, int length, int wage, int releaseClause, bool renewingContract)
{
    this->negotiatingPlayer = player;
    this->renewingContract = renewingContract;

    this->contractLength = length;
    this->contractWage = wage;
    this->contractReleaseClause = releaseClause;
}
