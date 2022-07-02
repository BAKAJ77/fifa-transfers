#include <states/main_menu.h>
#include <interface/menu_button.h>

void MainMenu::Init()
{
    // Initialize the member variables
    this->bkgOpacity = 0.0f;
    this->paused = false;

    // Initialize the user interface
    this->menuGUI = UserInterface(this->GetAppWindow(), 8.0f, 0.0f);

    this->menuGUI.AddStandaloneButton(new MenuButton({ 250, 960 }, { 450, 200 }, { 465, 215 }, "PLAY"));
    this->menuGUI.AddStandaloneButton(new MenuButton({ 720, 960 }, { 450, 200 }, { 465, 215 }, "SETTINGS"));
    this->menuGUI.AddStandaloneButton(new MenuButton({ 1190, 960 }, { 450, 200 }, { 465, 215 }, "HELP"));
    this->menuGUI.AddStandaloneButton(new MenuButton({ 1660, 960 }, { 450, 200 }, { 465, 215 }, "EXIT"));
}

void MainMenu::Destroy()
{
}

void MainMenu::Update(const float& deltaTime)
{
    if (!this->paused)
        this->menuGUI.Update(deltaTime);
}

void MainMenu::Render() const
{
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 2"),
        { 255, 255, 255, this->bkgOpacity });
    Renderer::GetInstance().RenderSquare({ 136, 52 }, { 232, 63 }, TextureLoader::GetInstance().GetTexture("Title Logo Small"),
        { 255, 255, 255, this->menuGUI.GetOpacity() });

    this->menuGUI.Render();
}

bool MainMenu::OnStartupTransitionUpdate(const float deltaTime)
{
    // Update background fade in effect
    this->bkgOpacity = std::min(this->bkgOpacity + (500.0f * deltaTime), 255.0f);

    // Once background fade in effect is complete, update the gui and logo fade in effect
    if (this->bkgOpacity == 255.0f)
    {
        this->menuGUI.SetOpacity(std::min(this->menuGUI.GetOpacity() + (500.0f * deltaTime), 255.0f));
        if (this->menuGUI.GetOpacity() == 255.0f)
            return true;
    }

    return false;
}

MainMenu* MainMenu::GetAppState()
{
    static MainMenu appState;
    return &appState;
}
