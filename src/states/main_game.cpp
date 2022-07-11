#include <states/main_game.h>

void MainGame::Init()
{
    // Initialize the member variables
    this->bkgOpacity = 0.0f;
}

void MainGame::Destroy()
{
}

void MainGame::Update(const float& deltaTime)
{
}

void MainGame::Render() const
{
    // Render the background
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 2"),
        { glm::vec3(255), this->bkgOpacity });

    // Render screen-wide bar at the top (this is where the buttons will be)
    Renderer::GetInstance().RenderSquare({ 960, 60 }, { 1920, 120 }, { glm::vec3(50), this->userInterface.GetOpacity() });

    // Render the small title logo
    Renderer::GetInstance().RenderSquare({ 169, 63 }, { 288, 78 }, TextureLoader::GetInstance().GetTexture("Title Logo Small"),
        { glm::vec3(255), this->userInterface.GetOpacity() });
}

bool MainGame::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update fade in effect of the background
    this->bkgOpacity = std::min(this->bkgOpacity + (transitionSpeed * deltaTime), 255.0f);

    if (this->bkgOpacity == 255.0f)
    {
        // Update the fade in effect of the user interface
        this->userInterface.SetOpacity(std::min(this->userInterface.GetOpacity() + (transitionSpeed * deltaTime), 255.0f));

        if (this->userInterface.GetOpacity() == 255.0f)
            return true;
    }

    return false;
}

MainGame* MainGame::GetAppState()
{
    static MainGame appState;
    return &appState;
}
