#include <states/splash_screen.h>
#include <states/resource_loader.h>

#include <core/input_system.h>
#include <util/timestamp.h>

void SplashScreen::Init()
{
    // Load the required texture
    TextureLoader::GetInstance().LoadFromFile("Background 1", "bkg.png");
    TextureLoader::GetInstance().LoadFromFile("Developer Logo", "dev_logo.png");
    TextureLoader::GetInstance().LoadFromFile("Title Logo Large", "title_logo_lrg.png");

    // Set the renderer clear color
    Renderer::GetInstance().SetClearColor({ 57, 57, 57, 255 });

    // Initialize member variables
    this->devLogoOpacity = this->titleLogoOpacity = 0.0f;
    this->devLogoEffectDone = this->startedResourceLoading = false;
}

void SplashScreen::Destroy()
{
    TextureLoader::GetInstance().Free("Developer Logo");
}

void SplashScreen::Update(const float& deltaTime)
{
    // Start the resource loading app state once the title logo has fully appeared
    if (this->titleLogoOpacity == 255.0f && !startedResourceLoading)
    {
        this->PushState(ResourceLoader::GetAppState());
        this->startedResourceLoading = true;
    }
}

void SplashScreen::Render() const
{
    // Render the background
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 1"));

    // Render the developer logo
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1000, 700 }, TextureLoader::GetInstance().GetTexture("Developer Logo"),
        { 255, 255, 255, this->devLogoOpacity });

    // Render the title logo
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1500, 375 }, TextureLoader::GetInstance().GetTexture("Title Logo Large"),
        { 255, 255, 255, this->titleLogoOpacity });
}

bool SplashScreen::OnStartupTransitionUpdate(const float deltaTime)
{
    // Update the dev logo splash screen effect
    this->UpdateDevLogoEffect(deltaTime);

    // Update title logo fade in effect
    if (this->devLogoEffectDone)
    {
        this->titleLogoOpacity = std::min(this->titleLogoOpacity + (300.0f * deltaTime), 255.0f);
        if (this->titleLogoOpacity == 255.0f)
            return true;
    }

    return false;
}

void SplashScreen::UpdateDevLogoEffect(const float& deltaTime)
{
    static bool fadeInDone = false; 

    if (!this->devLogoEffectDone)
    {
        if (this->devLogoOpacity < 255.0f && !fadeInDone)
            this->devLogoOpacity = std::min(this->devLogoOpacity + (300.0f * deltaTime), 255.0f);
        else
        {
            static float startTime = Util::GetSecondsSinceEpoch();
            fadeInDone = true;

            if (this->devLogoOpacity > 0.0f && Util::GetSecondsSinceEpoch() - startTime >= 2.0f)
                this->devLogoOpacity = std::max(this->devLogoOpacity - (300.0f * deltaTime), 0.0f);
            else if (this->devLogoOpacity == 0.0f)
            {
                static float delayStartTime = Util::GetSecondsSinceEpoch();
                if (Util::GetSecondsSinceEpoch() - delayStartTime >= 2.0f)
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
