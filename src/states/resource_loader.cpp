#include <states/resource_loader.h>
#include <states/splash_screen.h>
#include <util/timestamp.h>

#include <thread>
#include <mutex>

namespace Threading
{
    std::mutex mutex;
}

void ResourceLoader::Init()
{
    // Initialize member variables
    this->workDonePercentage = 0;
    this->splashScreenStarted = false;

    // Load the resources list JSON file
    this->resourcesListFile.Open("resources.json");

    // Load the application resources
    this->LoadTextures();
    this->LoadFonts();

    std::thread resourceLoadThread(&ResourceLoader::LoadAudio, this);
    resourceLoadThread.detach();
}

void ResourceLoader::Destroy() {}

void ResourceLoader::Update(const float& deltaTime)
{
    if (this->workDonePercentage == 100) // End this state once all resources have been loaded
    {
        static float workDoneTime = Util::GetSecondsSinceEpoch();
        if ((Util::GetSecondsSinceEpoch() - workDoneTime >= 2.0f) && !this->splashScreenStarted)
        {
            this->splashScreenStarted = true;
            this->PushState(SplashScreen::GetAppState());
        }
    }
}

void ResourceLoader::Render() const
{
    // Render the background
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 1"));

    if (!this->splashScreenStarted)
    {
        // Render the loading bar edge
        Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1000, 100 }, { 255, 255, 255, 255 });

        // Render the loading bar progress
        std::scoped_lock lock(Threading::mutex);
        Renderer::GetInstance().RenderSquare({ 470 + (490 * ((float)this->workDonePercentage / 100)), 540 },
            { (990 * ((float)this->workDonePercentage / 100)), 90 }, { 128, 128, 128, 255 });
    }
}

void ResourceLoader::LoadTextures()
{
    // Load all the textures
    uint32_t fetchIndex = 1;
    bool fetchedAllTextures = false;

    while (!fetchedAllTextures)
    {
        const std::string fetchID = std::to_string(fetchIndex++);
        
        if (this->resourcesListFile.GetRoot()["textures"].contains(fetchID))
        {
            TextureLoader::GetInstance().LoadFromFile(this->resourcesListFile.GetRoot()["textures"][fetchID]["id"].get<std::string>(),
                this->resourcesListFile.GetRoot()["textures"][fetchID]["filename"].get<std::string>());
        }
        else
            fetchedAllTextures = true;
    }
}

void ResourceLoader::LoadFonts()
{
    // Load all the fonts
    uint32_t fetchIndex = 1;
    bool fetchedAllFonts = false;

    while (!fetchedAllFonts)
    {
        const std::string fetchID = std::to_string(fetchIndex++);
        
        if (this->resourcesListFile.GetRoot()["fonts"].contains(fetchID))
        {
            FontLoader::GetInstance().LoadFromFile(this->resourcesListFile.GetRoot()["fonts"][fetchID]["id"].get<std::string>(),
                this->resourcesListFile.GetRoot()["fonts"][fetchID]["filename"].get<std::string>(),
                this->resourcesListFile.GetRoot()["fonts"][fetchID]["style"].get<uint32_t>());
        }
        else
            fetchedAllFonts = true;
    }
}

void ResourceLoader::LoadAudio()
{
    bool allAudioFetched = false, tracksFetched = false;
    uint32_t fetchIndex = 1;

    while (!allAudioFetched)
    {
        if (!tracksFetched)
        {
            // Load all the music tracks first
            const std::string fetchID = std::to_string(fetchIndex++);

            if (this->resourcesListFile.GetRoot()["tracks"].contains(fetchID))
            {
                AudioSystem::GetInstance().LoadFromFile(this->resourcesListFile.GetRoot()["tracks"][fetchID]["id"].get<std::string>(),
                    this->resourcesListFile.GetRoot()["tracks"][fetchID]["filename"].get<std::string>());
            }
            else
            {
                tracksFetched = true;
                fetchIndex = 1;
            }
        }
        else
        {
            // Once all music tracks have been loaded, load all the sound effects
            const std::string fetchID = std::to_string(fetchIndex++);

            if (this->resourcesListFile.GetRoot()["sfx"].contains(fetchID))
            {
                AudioSystem::GetInstance().LoadFromFile(this->resourcesListFile.GetRoot()["sfx"][fetchID]["id"].get<std::string>(),
                    this->resourcesListFile.GetRoot()["sfx"][fetchID]["filename"].get<std::string>());
            }
            else
                allAudioFetched = true;
        }
    }

    // Update the work done percentage counter
    {
        std::scoped_lock lock(Threading::mutex);
        this->workDonePercentage = 100;
    }
}

ResourceLoader* ResourceLoader::GetAppState()
{
    static ResourceLoader appState;
    return &appState;
}
