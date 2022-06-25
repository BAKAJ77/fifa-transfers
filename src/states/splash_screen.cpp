#include <states/splash_screen.h>

void SplashScreen::Init()
{
    this->texture = Memory::LoadImageFromFile("sparkle.png");
}

void SplashScreen::Destroy()
{
}

void SplashScreen::Update(const float& deltaTime)
{
}

void SplashScreen::Render() const
{
    Renderer::GetInstance().RenderSquare({ 500, 100 }, { 150, 150 }, this->texture, 35);
    Renderer::GetInstance().RenderTriangle({ 800, 400 }, { 200, 200 }, { 255, 0, 0, 255 }, 70);
    Renderer::GetInstance().RenderSquare({ 350, 800 }, { 100, 200 }, { 255, 0, 0, 255 }, 70);
}

SplashScreen* SplashScreen::GetAppState()
{
    static SplashScreen appState;
    return &appState;
}
