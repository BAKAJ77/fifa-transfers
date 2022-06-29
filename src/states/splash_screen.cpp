#include <states/splash_screen.h>
#include <core/input_system.h>

void SplashScreen::Init()
{
    TextureLoader::GetInstance().LoadFromFile("Background", "bkg.png");
    TextureLoader::GetInstance().LoadFromFile("Dev-Logo", "dev_logo.png");
}

void SplashScreen::Destroy()
{
}

void SplashScreen::Update(const float& deltaTime)
{
}

void SplashScreen::Render() const
{
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background"));
}

SplashScreen* SplashScreen::GetAppState()
{
    static SplashScreen appState;
    return &appState;
}
