#include <states/splash_screen.h>
#include <core/input_system.h>

void SplashScreen::Init()
{
    FontLoader::GetInstance().LoadFromFile("Bahnschrift", "bahnschrift.ttf");
    TextureLoader::GetInstance().LoadFromFile("Dev-Logo", "dev_logo.png");

    Renderer::GetInstance().SetClearColor({ 128, 128, 128, 255 });
}

void SplashScreen::Destroy()
{
}

void SplashScreen::Update(const float& deltaTime)
{
    if (InputSystem::GetInstance().WasKeyPressed(KeyCode::KEY_ESCAPE))
        this->PopState();
}

void SplashScreen::Render() const
{
    Renderer::GetInstance().RenderText({ 600, 100 }, { 0, 255, 0, 255 }, FontLoader::GetInstance().GetFont("Bahnschrift"), 72, 
        "Andrija Zivkovic");
    Renderer::GetInstance().RenderTriangle({ 800, 400 }, { 200, 200 }, { 255, 0, 0, 255 }, 70);
    Renderer::GetInstance().RenderSquare({ 350, 800 }, { 100, 200 }, { 255, 0, 0, 255 }, 70);

    Renderer::GetInstance().RenderSquare({ 300, 500 }, { 1000, 600 }, TextureLoader::GetInstance().GetTexture("Dev-Logo"));
}

SplashScreen* SplashScreen::GetAppState()
{
    static SplashScreen appState;
    return &appState;
}
