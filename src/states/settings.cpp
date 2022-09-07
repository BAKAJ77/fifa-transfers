#include <states/settings.h>
#include <states/main_menu.h>
#include <core/application_core.h>
#include <interface/menu_button.h>

void Settings::Init()
{
    // Initialize the member variables
    this->exitState = false;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Load the configuration json file
    this->configFile.Open("config.json");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
    this->userInterface.AddButton(new MenuButton({ 1745, 1005 }, { 300, 100 }, { 315, 115 }, "BACK"));

    this->userInterface.AddTextField("Resolution Width", TextInputField({ 425, 285 }, { 120, 60 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES, 255, 2.5f, 
        std::to_string(this->configFile.GetRoot()["window"]["resolution"].get<std::vector<int>>()[0])));

    this->userInterface.AddTextField("Resolution Height", TextInputField({ 435, 365 }, { 120, 60 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES, 255, 2.5f,
        std::to_string(this->configFile.GetRoot()["window"]["resolution"].get<std::vector<int>>()[1])));

    this->userInterface.AddTickBox("Fullscreen", TickBox({ 30, 450 }, glm::vec2(40), "Enable Fullscreen Mode", 255, 0,
        this->configFile.GetRoot()["window"]["fullscreen"].get<bool>()));

    this->userInterface.AddTickBox("Vsync", TickBox({ 30, 530 }, glm::vec2(40), "Enable Vsync Mode", 255, 15,
        this->configFile.GetRoot()["window"]["vsync"].get<bool>()));

    this->userInterface.AddTextField("Text Resolution", TextInputField({ 390, 745 }, { 100, 60 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES, 255, 2.5f, 
        std::to_string(this->configFile.GetRoot()["graphics"]["textResolutionLevel"].get<int>())));

    this->userInterface.AddTextField("Samples Per Pixel", TextInputField({ 575, 825 }, { 85, 60 },
        TextInputField::Restrictions::NO_ALPHABETIC | TextInputField::Restrictions::NO_SPACES, 255, 2.5f, 
        std::to_string(this->configFile.GetRoot()["graphics"]["samplesMSAA"].get<int>())));
}

void Settings::Destroy()
{
    // Update the config settings loaded
    this->configFile.GetRoot()["window"]["resolution"] = {
        std::stoi(this->userInterface.GetTextField("Resolution Width")->GetInputtedText()),
        std::stoi(this->userInterface.GetTextField("Resolution Height")->GetInputtedText()) };

    this->configFile.GetRoot()["window"]["fullscreen"] = this->userInterface.GetTickBox("Fullscreen")->isCurrentlyTicked();
    this->configFile.GetRoot()["window"]["vsync"] = this->userInterface.GetTickBox("Vsync")->isCurrentlyTicked();

    this->configFile.GetRoot()["graphics"]["textResolutionLevel"] =
        std::stoi(this->userInterface.GetTextField("Text Resolution")->GetInputtedText());

    this->configFile.GetRoot()["graphics"]["samplesMSAA"] =
        std::stoi(this->userInterface.GetTextField("Samples Per Pixel")->GetInputtedText());

    // Write the settings to the config file, then close
    this->configFile.Close();
}

void Settings::Update(const float& deltaTime)
{
    if (!this->exitState)
    {
        // Update the user interface
        this->userInterface.Update(deltaTime);

        // Check if any of othe buttons has been clicked
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

void Settings::Render() const
{
    // Render the small title logo
    Renderer::GetInstance().RenderSquare({ 169, 63 }, { 288, 78 }, TextureLoader::GetInstance().GetTexture("Title Logo Small"),
        { glm::vec3(255), this->logoOpacity });

    // Render the app state title
    Renderer::GetInstance().RenderShadowedText({ 1550, 90 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75,
        "SETTINGS", 5);

    // Render setting labels
    Renderer::GetInstance().RenderShadowedText({ 30, 220 }, { 0, 150, 150, this->userInterface.GetOpacity() }, this->font, 60,
        "WINDOW CONFIGURATIONS", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 300 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Resolution Width", 5);
    Renderer::GetInstance().RenderShadowedText({ 30, 380 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Resolution Height", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 680 }, { 0, 150, 150, this->userInterface.GetOpacity() }, this->font, 60,
        "GRAPHICS CONFIGURATIONS", 5);

    Renderer::GetInstance().RenderShadowedText({ 30, 760 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Text Resolution", 5);
    Renderer::GetInstance().RenderShadowedText({ 30, 840 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 40,
        "Samples Per Pixel (MSAA)", 5);

    // Render warning text
    Renderer::GetInstance().RenderShadowedText({ 30, 1030 }, { 255, 0, 0, this->userInterface.GetOpacity() }, this->font, 40,
        "NOTE: You'll need to restart the application for your changes to take effect.", 5);

    // Render the user interface
    this->userInterface.Render();
}

bool Settings::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

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

Settings* Settings::GetAppState()
{
    static Settings appState;
    return &appState;
}
