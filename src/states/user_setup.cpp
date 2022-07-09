#include <states/user_setup.h>
#include <states/save_writing.h>

#include <core/input_system.h>
#include <serialization/save_data.h>
#include <interface/menu_button.h>

void UserSetup::Init()
{
    // Initialize the app state member variables
    this->lastSelectedLeague = -1;
    this->goBack = this->managerNameInvalid = this->noLeagueSelected = this->noClubSelected = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddStandaloneButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));
    this->userInterface.AddStandaloneTextField("Manager Name", TextInputField({ 330, 295 }, { 600, 75 }));

    this->userInterface.AddDropDown("League", { { 280, 515 }, { 500, 75 } });

    for (size_t i = 0; i < SaveData::GetInstance().GetLeagueDatabase().size(); i++)
        this->userInterface.GetDropDown("League")->AddSelection(SaveData::GetInstance().GetLeagueDatabase()[i].GetName(), 
            (int)SaveData::GetInstance().GetLeagueDatabase()[i].GetID());

    this->userInterface.AddDropDown("Club", { { 280, 735 }, { 500, 75 } });
}

void UserSetup::Destroy() {}

bool UserSetup::CheckInputsValid()
{
    this->managerNameInvalid = this->userInterface.GetStandaloneTextField("Manager Name")->GetInputtedText().empty();
    this->noLeagueSelected = this->userInterface.GetDropDown("League")->GetCurrentSelected() == -1;
    this->noClubSelected = this->userInterface.GetDropDown("Club")->GetCurrentSelected() == -1;

    return !this->managerNameInvalid && !this->noLeagueSelected && !this->noClubSelected;
}

void UserSetup::UpdateClubDropDownList()
{
    if (this->lastSelectedLeague != this->userInterface.GetDropDown("League")->GetCurrentSelected())
    {
        this->userInterface.GetDropDown("Club")->Clear(); // Clear the previous element list

        // Fetch the clubs in the current selected league
        const std::vector<Club>& clubDatabase = SaveData::GetInstance().GetClubDatabase();
        for (size_t i = 0; i < clubDatabase.size(); i++)
        {
            if (clubDatabase[i].GetLeague() == this->userInterface.GetDropDown("League")->GetCurrentSelected())
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

        this->lastSelectedLeague = this->userInterface.GetDropDown("League")->GetCurrentSelected();
    }
}

UserProfile UserSetup::SetupUserProfile(uint16_t id, const std::string_view& name, Club& club)
{
    // Create the new user profile
    UserProfile user = { id, name, club };

    // Setup new competition tracking data
    user.AddCompetitionData({ 1, (uint16_t)this->userInterface.GetDropDown("League")->GetCurrentSelected() });

    const League* selectedLeague =
        SaveData::GetInstance().GetLeague((uint16_t)this->userInterface.GetDropDown("League")->GetCurrentSelected());

    for (size_t i = 0; i < SaveData::GetInstance().GetCupDatabase().size(); i++)
    {
        bool clubCompatibleWithCup = false;
        for (const std::string& nation : SaveData::GetInstance().GetCupDatabase()[i].GetNations())
        {
            if (selectedLeague->GetNation() == nation)
            {
                clubCompatibleWithCup = true;
                break;
            }
        }

        if (clubCompatibleWithCup)
            user.AddCompetitionData({ (uint16_t)(i + 2), SaveData::GetInstance().GetCupDatabase()[i].GetID() });
    }

    return user;
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
        const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

        for (size_t index = 0; index < buttons.size(); index++)
        {
            const MenuButton* button = (MenuButton*)buttons[index];
            if (button->GetText() == "CONFIRM" && button->WasClicked())
            {
                if (this->CheckInputsValid()) // Make sure the given inputs are valid
                {
                    // Retrieve the object of the club that was selected
                    for (auto& club : SaveData::GetInstance().GetClubDatabase())
                    {
                        if (this->userInterface.GetDropDown("Club")->GetCurrentSelected() == club.GetID())
                        {
                            // Setup the new user profile
                            UserProfile user = this->SetupUserProfile((uint16_t)(SaveData::GetInstance().GetUsers().size() + 1),
                                this->userInterface.GetStandaloneTextField("Manager Name")->GetInputtedText(), club);

                            // Push the created user profile into the save data
                            SaveData::GetInstance().GetUsers().emplace_back(user);
                            break;
                        }
                    }

                    // Reset the drop downs and text field
                    this->userInterface.GetStandaloneTextField("Manager Name")->Clear();
                    this->userInterface.GetDropDown("League")->Reset();
                    this->userInterface.GetDropDown("Club")->Reset();

                    // Write the save data to new save file
                    if (SaveData::GetInstance().GetUsers().size() == SaveData::GetInstance().GetPlayerCount())
                        this->SwitchState(SaveWriting::GetAppState());
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
    Renderer::GetInstance().RenderShadowedText({ 25, 225 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "Enter your manager name:", 5);

    Renderer::GetInstance().RenderShadowedText({ 25, 440 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "Select your preferred league:", 5);

    Renderer::GetInstance().RenderShadowedText({ 25, 655 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "Select your preferred club:", 5);

    // Render any input validation errors that occur
    if (this->managerNameInvalid)
        Renderer::GetInstance().RenderText({ 660, 310 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30,
            "*Make sure you've entered a manager name that isn't already in use.");

    if (this->noLeagueSelected)
        Renderer::GetInstance().RenderText({ 560, 530 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30,
            "*An option in the drop down list must be selected.");

    if (this->noClubSelected)
        Renderer::GetInstance().RenderText({ 560, 750 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30,
            "*An option in the drop down list must be selected.");

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
