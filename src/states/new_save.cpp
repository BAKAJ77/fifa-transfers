#include <states/new_save.h>
#include <states/main_menu.h>
#include <states/user_setup.h>

#include <interface/menu_button.h>
#include <serialization/save_data.h>
#include <serialization/json_loader.h>
#include <util/directory_system.h>

void NewSave::Init()
{
    // Initialize the member variables
    this->goBackToPlayMenu = this->saveNameInvalid = this->playerCountInvalid = this->growthSystemInvalid = this->randomisePotentialInvalid = 
        this->selectedLeagueInvalid = this->loadedDefaultDatabase = false;

    this->logoOpacity = 0.0f;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddTextField("Save Name", TextInputField({ 330, 245 }, { 600, 75 }));
    this->userInterface.AddTextField("Player Count", TextInputField({ 70, 465 }, { 75, 75 }, 
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));

    this->userInterface.AddRadioButtonGroup("Growth System", RadioButtonGroup({ 50, 665 }, { 50, 50 }));
    this->userInterface.GetRadioButtonGroup("Growth System")->Add("Overall Rating", 0);
    this->userInterface.GetRadioButtonGroup("Growth System")->Add("Skill Points", 1);

    this->userInterface.AddRadioButtonGroup("Randomise Potentials", RadioButtonGroup({ 50, 885 }, { 50, 50 }));
    this->userInterface.GetRadioButtonGroup("Randomise Potentials")->Add("Yes", 1);
    this->userInterface.GetRadioButtonGroup("Randomise Potentials")->Add("No", 0);

    this->userInterface.AddDropDown("League", DropDown({ 1500, 245 }, { 600, 75 }));
}

void NewSave::Destroy() {}

void NewSave::Update(const float& deltaTime)
{
    if (!this->goBackToPlayMenu)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Get the interface standalone buttons
        const std::vector<ButtonBase*>& buttons = this->userInterface.GetButtons();

        // Check and respond if any of the standalone buttons are pressed
        for (size_t index = 0; index < buttons.size(); index++)
        {
            const MenuButton* menuButton = (MenuButton*)buttons[index];
            if (menuButton->GetText() == "CONFIRM" && menuButton->WasClicked())
            {
                // Retrieve the inputted data
                const std::string& saveNameStr = this->userInterface.GetTextField("Save Name")->GetInputtedText();
                const std::string& playerCountStr = this->userInterface.GetTextField("Player Count")->GetInputtedText();
                const int growthSystemType = this->userInterface.GetRadioButtonGroup("Growth System")->GetSelected();
                const int selectedLeagueID = this->userInterface.GetDropDown("League")->GetCurrentSelected();
                
                this->randomisePotentials = this->userInterface.GetRadioButtonGroup("Randomise Potentials")->GetSelected();

                // Make sure all the data entered is valid
                this->saveNameInvalid = saveNameStr.empty();
                
                JSONLoader savesFile("data/saves.json");
                uint16_t id = 0;

                while (savesFile.GetRoot().contains(std::to_string(id))) 
                {
                    if (savesFile.GetRoot()[std::to_string(id)]["filename"].get<std::string>() == (saveNameStr + ".json"))
                    {
                        this->saveNameInvalid = true;
                        break;
                    }

                    ++id;
                }

                this->growthSystemInvalid = growthSystemType == -1;
                this->randomisePotentialInvalid = this->randomisePotentials == -1;

                if (playerCountStr.empty())
                    this->playerCountInvalid = true;
                else if (std::stoi(playerCountStr) < 1 || std::stoi(playerCountStr) > 8)
                    this->playerCountInvalid = true;
                else
                    this->playerCountInvalid = false;

                this->selectedLeagueInvalid = selectedLeagueID == -1;
                
                // Once all the inputted data is valid, continue onto the next steps
                if (!this->saveNameInvalid && !this->playerCountInvalid && !this->growthSystemInvalid && !this->randomisePotentialInvalid &&
                    !this->selectedLeagueInvalid)
                {
                    // Assign all the retrieved values into the new save data
                    SaveData::GetInstance().SetSaveName(saveNameStr);
                    SaveData::GetInstance().SetPlayerCount((uint8_t)std::stoi(playerCountStr));
                    SaveData::GetInstance().SetGrowthSystem((SaveData::GrowthSystemType)growthSystemType);
                    SaveData::GetInstance().SetCurrentYear(2024);

                    SaveData::GetInstance().SetCurrentLeague(SaveData::GetInstance().GetLeague((uint16_t)selectedLeagueID));

                    // Finally, continue onto the user setup app state
                    this->PushState(UserSetup::GetAppState());
                }
            }
            else if (menuButton->GetText() == "BACK" && menuButton->WasClicked())
                this->goBackToPlayMenu = true;
        }
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the interface and small title logo fade out effect
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        this->logoOpacity = this->userInterface.GetOpacity();

        if (this->userInterface.GetOpacity() == 0.0f)
        {
            // Update large title logo fade in effect
            MainMenu::GetAppState()->SetLogoOpacity(std::min(MainMenu::GetAppState()->GetLogoOpacity() + (transitionSpeed * deltaTime), 255.0f));
            if (MainMenu::GetAppState()->GetLogoOpacity() == 255.0f)
                this->PopState();
        }
    }
}

void NewSave::Render() const
{
    // Render the small title logo
    Renderer::GetInstance().RenderSquare({ 169, 63 }, { 288, 78 }, TextureLoader::GetInstance().GetTexture("Title Logo Small"), 
        { glm::vec3(255), this->logoOpacity });

    // Render the text descriptions
    Renderer::GetInstance().RenderShadowedText({ 30, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter a name for this save:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 390 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Enter the number of users in this save (between 1 to 8):", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 605 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Choose the growth system to use in this save:", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 820 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Do you want to randomise the potentials of players in this save?", 5);

    Renderer::GetInstance().RenderShadowedText({ 1200, 175 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Select your preferred league", 5);

    // Render any input validation errors that occur
    if (this->saveNameInvalid)
        Renderer::GetInstance().RenderText({ 660, 260 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->playerCountInvalid)
        Renderer::GetInstance().RenderText({ 137, 480 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->growthSystemInvalid)
        Renderer::GetInstance().RenderText({ 820, 700 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->randomisePotentialInvalid)
        Renderer::GetInstance().RenderText({ 365, 920 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    if (this->selectedLeagueInvalid)
        Renderer::GetInstance().RenderText({ 1830, 260 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30, "*");

    // Render the user interface
    this->userInterface.Render();
}

bool NewSave::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    if (!this->loadedDefaultDatabase)
    {
        // Clear the data already in the databases before loading
        SaveData::GetInstance().GetPlayerDatabase().clear();
        SaveData::GetInstance().GetClubDatabase().clear();
        SaveData::GetInstance().GetLeagueDatabase().clear();
        SaveData::GetInstance().GetUsers().clear();
        SaveData::GetInstance().GetNegotiationCooldowns().clear();
        SaveData::GetInstance().GetTransferHistory().clear();

        // Load the default data from the player and club database json files
        JSONLoader playersFile("data/players.json");
        JSONLoader clubsFile("data/clubs.json");
        JSONLoader leaguesFile("data/leagues.json");

        SaveData::GetInstance().LoadPlayersFromJSON(playersFile.GetRoot());
        SaveData::GetInstance().LoadClubsFromJSON(clubsFile.GetRoot());
        SaveData::GetInstance().LoadLeaguesFromJSON(leaguesFile.GetRoot());

        // Fetch all the supported leagues in the database
        for (size_t i = 0; i < SaveData::GetInstance().GetLeagueDatabase().size(); i++)
        {
            if (SaveData::GetInstance().GetLeagueDatabase()[i].IsSupported())
            {
                this->userInterface.GetDropDown("League")->AddSelection(SaveData::GetInstance().GetLeagueDatabase()[i].GetName(),
                    (int)SaveData::GetInstance().GetLeagueDatabase()[i].GetID());
            }
        }

        this->loadedDefaultDatabase = true;
    }

    // Update large title logo fade out effect
    MainMenu::GetAppState()->SetLogoOpacity(std::max(MainMenu::GetAppState()->GetLogoOpacity() - (transitionSpeed * deltaTime), 0.0f));

    // Once the large title logo fade out effect is complete, update interface and small title logo fade in effect
    if (MainMenu::GetAppState()->GetLogoOpacity() == 0.0f)
    {
        this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
        this->logoOpacity = this->userInterface.GetOpacity();

        if (this->userInterface.GetOpacity() == 255.0f)
            return true;
    }

    return false;
}

bool NewSave::OnPauseTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the interface fade out effect
    this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
    if (this->userInterface.GetOpacity() == 0.0f)
        return true;

    return false;
}

bool NewSave::OnResumeTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update the interface fade in effect
    this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
    if (this->userInterface.GetOpacity() == 255.0f)
        return true;

    return false;
}

NewSave* NewSave::GetAppState()
{
    static NewSave appState;
    return &appState;
}

bool NewSave::ShouldRandomisePotentials() const
{
    return (bool)this->randomisePotentials;
}
