#include <states/training_menu.h>
#include <states/main_game.h>
#include <interface/menu_button.h>
#include <util/data_manip.h>

void TrainingMenu::Init()
{
    // Initialize the member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Calculate the prices for the available staff to hire
    const UserProfile* currentUser = MainGame::GetAppState()->GetCurrentUser();

    constexpr int maxLevel = 4, baseCost = 5000;
    float costMultiplier = 1.0f, levelMultiplier = 0.5f;

    if (currentUser->GetClub()->GetAverageOverall() >= 80)
        costMultiplier = 12.0f;
    else if (currentUser->GetClub()->GetAverageOverall() >= 75)
        costMultiplier = 8.0f;
    else if (currentUser->GetClub()->GetAverageOverall() >= 70)
        costMultiplier = 5.0f;
    else if (currentUser->GetClub()->GetAverageOverall() >= 65)
        costMultiplier = 3.0f;
    else if (currentUser->GetClub()->GetAverageOverall() >= 60)
        costMultiplier = 2.0f;

    this->purchasableStaff.clear();

    for (int i = 0; i < maxLevel; i++)
    {
        this->purchasableStaff.push_back({ { Club::StaffType::GOALKEEPING, (i + 1) }, 
            Util::GetTruncatedSFInteger((int)(baseCost * costMultiplier * levelMultiplier), 3) });

        this->purchasableStaff.push_back({ { Club::StaffType::DEFENCE, (i + 1) }, 
            Util::GetTruncatedSFInteger((int)(baseCost * costMultiplier * levelMultiplier), 3) });

        this->purchasableStaff.push_back({ { Club::StaffType::MIDFIELD, (i + 1) }, 
            Util::GetTruncatedSFInteger((int)(baseCost * costMultiplier * levelMultiplier), 3) });

        this->purchasableStaff.push_back({ { Club::StaffType::ATTACK, (i + 1) }, 
            Util::GetTruncatedSFInteger((int)(baseCost * costMultiplier * levelMultiplier), 3) });

        levelMultiplier += 0.5f;
    }

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddSelectionList("Training Staff", SelectionList({ 960, 490 }, { 1860, 720 }, 80, 255, 30));
    this->userInterface.GetSelectionList("Training Staff")->AddCategory("Name");
    this->userInterface.GetSelectionList("Training Staff")->AddCategory("Quality Level");
    this->userInterface.GetSelectionList("Training Staff")->AddCategory("Price");
    this->userInterface.GetSelectionList("Training Staff")->AddCategory("Currently Hired?");

    this->LoadAvailableStaff(currentUser);
}

void TrainingMenu::Destroy() {}

void TrainingMenu::LoadAvailableStaff(const UserProfile* currentUser)
{
    this->userInterface.GetSelectionList("Training Staff")->Clear();

    for (size_t index = 0; index < this->purchasableStaff.size(); index++)
    {
        const AvailableStaff& staff = this->purchasableStaff[index];

        if (currentUser->GetClub()->GetTrainingStaff(staff.details.type).level == 0 ||
            staff.details.level == currentUser->GetClub()->GetTrainingStaff(staff.details.type).level)
        {
            std::string name, price, isCurrentlyHired;

            if (staff.details.type == Club::StaffType::GOALKEEPING)
                name = "Goalkeeping Training Staff";
            else if (staff.details.type == Club::StaffType::DEFENCE)
                name = "Defensive Training Staff";
            else if (staff.details.type == Club::StaffType::MIDFIELD)
                name = "Midfield Training Staff";
            else if (staff.details.type == Club::StaffType::ATTACK)
                name = "Attacking Training Staff";

            if (staff.details.level == currentUser->GetClub()->GetTrainingStaff(staff.details.type).level)
            {
                price = "N/A";
                isCurrentlyHired = "Yes";

                this->userInterface.GetSelectionList("Training Staff")->AddElement({ name, std::to_string(staff.details.level), price, isCurrentlyHired }, -1,
                    { 0, 85, 85 }, { 0, 115, 155 }, { 0, 60, 60 });
            }
            else
            {
                price = Util::GetFormattedCashString(staff.price);
                isCurrentlyHired = "No";

                this->userInterface.GetSelectionList("Training Staff")->AddElement({ name, std::to_string(staff.details.level), price, isCurrentlyHired }, (int)index);
            }
        }
    }
}

void TrainingMenu::Update(const float& deltaTime)
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

        // Check if any of the staff in the selection list have been selected
        if (this->userInterface.GetSelectionList("Training Staff")->GetCurrentSelected() != -1)
        {
            const AvailableStaff& trainingStaff = this->purchasableStaff[this->userInterface.GetSelectionList("Training Staff")->GetCurrentSelected()];

            if (MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetWageBudget() >= trainingStaff.price)
            {
                MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetTrainingStaff(trainingStaff.details.type).level = trainingStaff.details.level;
                MainGame::GetAppState()->GetCurrentUser()->GetClub()->SetWageBudget(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetWageBudget() -
                    trainingStaff.price);

                this->LoadAvailableStaff(MainGame::GetAppState()->GetCurrentUser());
            }

            this->userInterface.GetSelectionList("Training Staff")->Reset();
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

void TrainingMenu::Render() const
{
    // Render the app state title
    Renderer::GetInstance().RenderShadowedText({ 1130, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "HIRE TRAINING STAFF", 5);

    // Render the user's transfer budget
    Renderer::GetInstance().RenderShadowedText({ 30, 1035 }, { 0, 200, 200, this->userInterface.GetOpacity() }, this->font, 75,
        "WAGE BUDGET: " + Util::GetFormattedCashString(MainGame::GetAppState()->GetCurrentUser()->GetClub()->GetWageBudget()), 5);

    // Render the user interface
    this->userInterface.Render();
}

bool TrainingMenu::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the interface fade in effect
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

TrainingMenu* TrainingMenu::GetAppState()
{
    static TrainingMenu appState;
    return &appState;
}
