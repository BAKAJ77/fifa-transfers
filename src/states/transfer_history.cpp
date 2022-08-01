#include <states/transfer_history.h>
#include <states/main_game.h>
#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <util/data_manip.h>

void TransferHistory::Init()
{
    // Initialize the member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddSelectionList("Transfer History", { { 960, 490 }, { 1860, 720 }, 80, 255, 25 });
    this->userInterface.GetSelectionList("Transfer History")->AddCategory("Name");
    this->userInterface.GetSelectionList("Transfer History")->AddCategory("From");
    this->userInterface.GetSelectionList("Transfer History")->AddCategory("To");
    this->userInterface.GetSelectionList("Transfer History")->AddCategory("Transfer Fee");

    // Load the transfer history into the list (from newest to oldest hence using the reverse iterator)
    const std::vector<SaveData::PastTransfer>& transferHistory = SaveData::GetInstance().GetTransferHistory();

    for (auto iterator = transferHistory.rbegin(); iterator != transferHistory.rend(); iterator++)
    {
        const Player* player = SaveData::GetInstance().GetPlayer(iterator->playerID);
        const Club* fromClub = SaveData::GetInstance().GetClub(iterator->fromClubID);
        const Club* toClub = SaveData::GetInstance().GetClub(iterator->toClubID);

        if ((iterator->fromClubID == fromClub->GetID()) || iterator->toClubID == toClub->GetID())
        {
            this->userInterface.GetSelectionList("Transfer History")->AddElement({ player->GetName().data(), fromClub->GetName().data(), 
                toClub->GetName().data(), Util::GetFormattedCashString(iterator->transferFee) }, -1);
        }
    }
}

void TransferHistory::Destroy() {}

void TransferHistory::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any buttons have been clicked
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

void TransferHistory::Render() const
{
    // Render the app state title
    Renderer::GetInstance().RenderShadowedText({ 1210, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "TRANSFER HISTORY", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool TransferHistory::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the fade in effect of the user interface
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

TransferHistory* TransferHistory::GetAppState()
{
    static TransferHistory appState;
    return &appState;
}
