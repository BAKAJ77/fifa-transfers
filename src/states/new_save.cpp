#include <states/new_save.h>
#include <states/main_menu.h>
#include <interface/menu_button.h>
#include <serialization/save_data.h>

void NewSave::Init()
{
    // Initialize the member variables
    this->goBackToPlayMenu = this->saveNameInvalid = this->playerCountInvalid = this->growthSystemInvalid = this->randomisePotentialInvalid = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddStandaloneButton(new MenuButton({ 1420, 1005 }, { 300, 100 }, { 315, 115 }, "CONFIRM"));
    this->userInterface.AddStandaloneButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddStandaloneTextField("Save Name", TextInputField({ 330, 295 }, { 600, 75 }));
    this->userInterface.AddStandaloneTextField("Player Count", TextInputField({ 70, 515 }, { 75, 75 }, 
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES));

    this->userInterface.AddRadioButtonGroup("Growth System", RadioButtonGroup({ 50, 735 }, { 50, 50 }));
    this->userInterface.GetRadioButtonGroup("Growth System")->Add("Overall Rating", 0);
    this->userInterface.GetRadioButtonGroup("Growth System")->Add("Skill Points", 1);

    this->userInterface.AddRadioButtonGroup("Randomise Potentials", RadioButtonGroup({ 50, 955 }, { 50, 50 }));
    this->userInterface.GetRadioButtonGroup("Randomise Potentials")->Add("Yes", 1);
    this->userInterface.GetRadioButtonGroup("Randomise Potentials")->Add("No", 0);
}

void NewSave::Destroy() 
{
    this->userInterface.GetStandaloneTextField("Save Name")->Clear();
    this->userInterface.GetStandaloneTextField("Player Count")->Clear();
}

void NewSave::Update(const float& deltaTime)
{
    if (!this->goBackToPlayMenu)
    {
        this->userInterface.Update(deltaTime);

        // Get the interface standalone buttons
        const std::vector<ButtonBase*>& buttons = this->userInterface.GetStandaloneButtons();

        // Check and respond if any of the standalone buttons are pressed
        for (size_t index = 0; index < buttons.size(); index++)
        {
            const MenuButton* menuButton = (MenuButton*)buttons[index];
            if (menuButton->GetText() == "CONFIRM" && menuButton->WasClicked())
            {
                // Retrieve the inputted data
                const std::string& saveNameStr = this->userInterface.GetStandaloneTextField("Save Name")->GetInputtedText();
                const std::string& playerCountStr = this->userInterface.GetStandaloneTextField("Player Count")->GetInputtedText();
                const int growthSystemType = this->userInterface.GetRadioButtonGroup("Growth System")->GetSelected();
                this->randomisePotentials = this->userInterface.GetRadioButtonGroup("Randomise Potentials")->GetSelected();

                // Make sure all the data entered is valid
                this->saveNameInvalid = saveNameStr.empty();
                this->growthSystemInvalid = growthSystemType == -1;
                this->randomisePotentialInvalid = this->randomisePotentials == -1;

                if (playerCountStr.empty())
                    this->playerCountInvalid = true;
                else if (std::stoi(playerCountStr) < 1 || std::stoi(playerCountStr) > 8)
                    this->playerCountInvalid = true;
                else
                    this->playerCountInvalid = false;
                
                // Once all the inputted data is valid, continue onto the next steps
                if (!this->saveNameInvalid && !this->playerCountInvalid && !this->growthSystemInvalid && !this->randomisePotentialInvalid)
                {
                    // Assign all the retrieved values into the new save data
                    SaveData::GetInstance().SetSaveName(saveNameStr);
                    SaveData::GetInstance().SetPlayerCount((uint8_t)std::stoi(playerCountStr));
                    SaveData::GetInstance().SetGrowthSystem((SaveData::GrowthSystemType)growthSystemType);

                    // Finally, continue onto the user setup app state (TODO)
                }
            }
            else if (menuButton->GetText() == "BACK" && menuButton->WasClicked())
                this->goBackToPlayMenu = true;
        }
    }
    else
    {
        constexpr float transitionSpeed = 1000.0f;

        // Update the interface fade out effect
        this->userInterface.SetOpacity(std::max(this->userInterface.GetOpacity() - (transitionSpeed * deltaTime), 0.0f));
        if (this->userInterface.GetOpacity() == 0.0f)
        {
            // Update title logo fade in effect
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
        { glm::vec3(255), this->userInterface.GetOpacity() });

    // Render the text field descriptions
    Renderer::GetInstance().RenderShadowedText({ 25, 225 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "Enter a name for this save:", 5);

    Renderer::GetInstance().RenderShadowedText({ 25, 440 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "Enter the number of users in this save (between 1 to 8):", 5);

    Renderer::GetInstance().RenderShadowedText({ 25, 655 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "Choose the growth system to use in this save:", 5);

    Renderer::GetInstance().RenderShadowedText({ 25, 870 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 50,
        "Do you want to randomise the potentials of players in this save?", 5);

    // Render any input validation errors that occur
    if (this->saveNameInvalid)
        Renderer::GetInstance().RenderText({ 660, 310 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30,
            "*At least one character must be entered for the save name.");

    if (this->playerCountInvalid)
        Renderer::GetInstance().RenderText({ 137, 530 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30,
            "*Make sure you've entered a number and it is between 1 and 8.");

    if (this->growthSystemInvalid)
        Renderer::GetInstance().RenderText({ 820, 750 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30,
            "*You have to select one of these options.");

    if (this->randomisePotentialInvalid)
        Renderer::GetInstance().RenderText({ 365, 970 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 30,
            "*You have to select one of these options.");

    // Render the user interface
    this->userInterface.Render();
}

bool NewSave::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update title logo fade out effect
    MainMenu::GetAppState()->SetLogoOpacity(std::max(MainMenu::GetAppState()->GetLogoOpacity() - (transitionSpeed * deltaTime), 0.0f));

    // Once the title logo fade out effect is complete, update interface fade in effect
    if (MainMenu::GetAppState()->GetLogoOpacity() == 0.0f)
    {
        this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));
        if (this->userInterface.GetOpacity() == 255.0f)
            return true;
    }

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
