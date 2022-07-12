#include <states/load_save.h>
#include <states/main_menu.h>

void LoadSave::Init()
{
    // Initialize the member variables
    this->goBackToPlayMenu = false;
    this->logoOpacity = 0.0f;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Initialize the user interface
    this->userInterface = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);
}

void LoadSave::Destroy()
{
}

void LoadSave::Update(const float& deltaTime)
{
}

void LoadSave::Render() const
{
    // Render the small title logo
    Renderer::GetInstance().RenderSquare({ 169, 63 }, { 288, 78 }, TextureLoader::GetInstance().GetTexture("Title Logo Small"),
        { glm::vec3(255), this->logoOpacity });

    // Render the text description
    Renderer::GetInstance().RenderShadowedText({ 620, 180 }, { glm::vec3(255), this->userInterface.GetOpacity() }, this->font, 75, 
        "Select a save below:", 5);
}

bool LoadSave::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update large title logo fade out effect
    MainMenu::GetAppState()->SetLogoOpacity(std::max(MainMenu::GetAppState()->GetLogoOpacity() - (transitionSpeed * deltaTime), 0.0f));

    if (MainMenu::GetAppState()->GetLogoOpacity() == 0.0f)
    {
        // Update fade in effect of the title logo
        this->logoOpacity = std::min(this->logoOpacity + (transitionSpeed * deltaTime), 255.0f);

        if (this->logoOpacity == 255.0f)
        {
            // Update the fade in effect of the user interface
            this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));

            if (this->userInterface.GetOpacity() == 255.0f)
                return true;
        }
    }

    return false;
}

LoadSave* LoadSave::GetAppState()
{
    static LoadSave appState;
    return &appState;
}
