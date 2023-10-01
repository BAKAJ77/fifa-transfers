#include <states/user_setup.h>
#include <states/save_writing.h>
#include <states/main_game.h>

#include <core/input_system.h>
#include <serialization/save_data.h>
#include <interface/menu_button.h>

void UserSetup::Init()
{
    // Initialize the app state member variables
    this->goBack = this->managerNameInvalid = this->noClubSelected = false;
    this->fetchAvailableClubs = true;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddTextField("Manager Name", TextInputField({ 330, 245 }, { 600, 75 }));
    this->userInterface.AddDropDown("Club", { { 280, 465 }, { 500, 75 } });
}

void UserSetup::Destroy() {}

bool UserSetup::CheckInputsValid()
{
    this->managerNameInvalid = this->userInterface.GetTextField("Manager Name")->GetInputtedText().empty();
    this->noClubSelected = this->userInterface.GetDropDown("Club")->GetCurrentSelected() == -1;

    return !this->managerNameInvalid && !this->noClubSelected;
}

void UserSetup::UpdateClubDropDownList()
{
    if (this->fetchAvailableClubs)
    {
        this->userInterface.GetDropDown("Club")->Clear(); // Clear the previous element list

        // Fetch the clubs in the current selected league
        const std::vector<Club>& clubDatabase = SaveData::GetInstance().GetClubDatabase();
        for (size_t i = 0; i < clubDatabase.size(); i++)
        {
            if (clubDatabase[i].GetLeague() == SaveData::GetInstance().GetCurrentLeague()->GetID())
            {
                bool clubTaken = false;
                for (const UserProfile& user : SaveData::GetInstance().GetUsers())
                {
                    if (clubDatabase[i].GetID() == user.GetClub()->GetID())
                    {
                        clubTaken = true;
                        break;
                    }
                }

                if (!clubTaken)
                    this->userInterface.GetDropDown("Club")->AddSelection(clubDatabase[i].GetName(), (int)clubDatabase[i].GetID());
            }
        }

        this->fetchAvailableClubs = false;
    }
}

UserProfile UserSetup::SetupUserProfile(uint16_t id, const std::string_view& name, Club& club)
{
    std::vector<UserProfile::CompetitionData> competitionTrackingData;

    // Setup new competition tracking data
    const League* selectedLeague = SaveData::GetInstance().GetCurrentLeague();
    competitionTrackingData.push_back({ 0, selectedLeague->GetID() });

    for (size_t i = 0; i < selectedLeague->GetLinkedCompetitions().size(); i++)
        competitionTrackingData.push_back({ (uint16_t)(i + 1), selectedLeague->GetLinkedCompetitions()[i].competitionID });

    // Create the new user profile
    return UserProfile(id, name, club, competitionTrackingData);
}

void UserSetup::Update(const float& deltaTime)
{
    if (!this->goBack)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Update the available clubs drop down list if a league has been selected
        this->UpdateClubDropDownList();

        // Check and respond if any of the standalone buttons are pressed
        const std::vector<ButtonBase*>& buttons = this->userInterface.GetButtons();

        for (size_t index = 0; index < buttons.size(); index++)
        {
            const MenuButton* button = (MenuButton*)buttons[index];
            if (button->GetText() == "CONFIRM" && button->WasClicked())
            {
                if (this->CheckInputsValid()) // Make sure the given inputs are valid
                {
                    // Setup the new user profile
                    Club* club = SaveData::GetInstance().GetClub((uint16_t)this->userInterface.GetDropDown("Club")->GetCurrentSelected());

                    UserProfile user = this->SetupUserProfile((uint16_t)SaveData::GetInstance().GetUsers().size(),
                        this->userInterface.GetTextField("Manager Name")->GetInputtedText(), *club);

                    // Push the created user profile into the save data
                    SaveData::GetInstance().GetUsers().emplace_back(user);

                    // Reset the drop downs and text field
                    this->userInterface.GetTextField("Manager Name")->Clear();
                    this->userInterface.GetDropDown("Club")->Reset();

                    // Write the save data to new save file
                    if (SaveData::GetInstance().GetUsers().size() == SaveData::GetInstance().GetPlayerCount())
                    {
                        SaveWriting::GetAppState()->SetNextState(MainGame::GetAppState());
                        this->SwitchState(SaveWriting::GetAppState());
                    }

                    this->fetchAvailableClubs = true; // Fetch available clubs left to choose since one has been taken now by a user
                }
            }
            else if (button->GetText() == "BACK" && button->WasClicked())
            {
                if (!SaveData::GetInstance().GetUsers().empty())
                    SaveData::GetInstance().GetUsers().pop_back();
                else
                    this->goBack = true;
            }
        }
    }
    else
    {
        if (this->OnPauseTransitionUpdate(deltaTime))
            this->PopState();
    }
}

void UserSetup::Render() const
{
    // Render the user profile creation index
    Renderer::GetInstance().RenderShadowedText({ 1375, 80 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "User Profile #" + std::to_string(SaveData::GetInstance().GetUsers().size() + 1), 5);

    // Render the text descriptions
    Renderer::GetInstance().RenderShadowedText({ 30, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter your manager name:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Select your preferred club:", 5);

    // Render any input validation errors that occur
    if (this->managerNameInvalid)
        Renderer::GetInstance().RenderText({ 660, 260 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->noClubSelected)
        Renderer::GetInstance().RenderText({ 560, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    this->userInterface.Render();
}

bool UserSetup::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the interface fade in effect
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

bool UserSetup::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the interface fade out effect
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));

    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

UserSetup* UserSetup::GetAppState()
{
    static UserSetup appState;
    return &appState;
}
