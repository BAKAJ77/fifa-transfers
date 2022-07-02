#include <states/splash_screen.h>
#include <states/main_menu.h>

#include <core/input_system.h>
#include <util/timestamp.h>

void SplashScreen::Init()
{
    // Initialize member variables
    this->devLogoOpacity = 0.0f;
    this->devLogoEffectDone = false;
}

void SplashScreen::Destroy()
{
    TextureLoader::GetInstance().Free("Developer Logo");
}

void SplashScreen::Update(const float& deltaTime)
{
    // Start the main menu application state once the dev logo effect has finished
    if (this->devLogoEffectDone)
        this->SwitchState(MainMenu::GetAppState());
}

void SplashScreen::Render() const
{
    // Render the developer logo
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1000, 700 }, TextureLoader::GetInstance().GetTexture("Developer Logo"),
        { 255, 255, 255, this->devLogoOpacity });
}

bool SplashScreen::OnStartupTransitionUpdate(const float deltaTime)
{
    // Update the dev logo splash screen effect
    this->UpdateDevLogoEffect(deltaTime);

    // Update title logo fade in effect
    if (this->devLogoEffectDone)
        return true;

    return false;
}

void SplashScreen::UpdateDevLogoEffect(const float& deltaTime)
{
    static bool fadeInDone = false; 

    if (!this->devLogoEffectDone)
    {
        if (this->devLogoOpacity < 255.0f && !fadeInDone)
            this->devLogoOpacity = std::min(this->devLogoOpacity + (150.0f * deltaTime), 255.0f);
        else
        {
            static float startTime = Util::GetSecondsSinceEpoch();
            fadeInDone = true;

            if (this->devLogoOpacity > 0.0f && Util::GetSecondsSinceEpoch() - startTime >= 2.0f)
                this->devLogoOpacity = std::max(this->devLogoOpacity - (300.0f * deltaTime), 0.0f);
            else if (this->devLogoOpacity == 0.0f)
            {
                static float delayStartTime = Util::GetSecondsSinceEpoch();
                if (Util::GetSecondsSinceEpoch() - delayStartTime >= 1.0f)
                    this->devLogoEffectDone = true;
            }
        }
    }
}

SplashScreen* SplashScreen::GetAppState()
{
    static SplashScreen appState;
    return &appState;
}
