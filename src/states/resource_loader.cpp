#include <states/resource_loader.h>
#include <serialization/config.h>

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
        this->PopState();
}

void ResourceLoader::Render() const
{
    // Render the loading bar edge
    Renderer::GetInstance().RenderSquare({ 960, 900 }, { 1000, 100 }, { 255, 255, 255, 255 });

    // Render the loading bar progress
    std::scoped_lock lock(Threading::mutex);
    Renderer::GetInstance().RenderSquare({ 470 + (490 * ((float)this->workDonePercentage / 100)), 900 }, 
        { (980 * ((float)this->workDonePercentage / 100)), 80 }, { 128, 128, 128, 255 });
}

void ResourceLoader::LoadTextures()
{
    TextureLoader::GetInstance().LoadFromFile("Background", "bkg2.png");
}

void ResourceLoader::LoadAudio()
{
    // Load all music tracks mentioned in the tracks json file
    ConfigLoader trackConfigFile("tracks.json");
    bool fetchedAllTracks = false;
    uint32_t trackIndex = 1;

    while (!fetchedAllTracks)
    {
        const std::string trackID = "track " + std::to_string(trackIndex++);
        if (trackConfigFile.IsEntryExisting(trackID))
        {
            AudioSystem::GetInstance().LoadFromFile(trackConfigFile.GetElement<std::string>("name", trackID), 
                trackConfigFile.GetElement<std::string>("filename", trackID));
        }
        else
            fetchedAllTracks = true;
    }

    // Update the work done percentage counter
    {
        std::scoped_lock lock(Threading::mutex);
        this->workDonePercentage = 100;
    }
}

void ResourceLoader::LoadFonts()
{
    // Load all font assets
    FontLoader::GetInstance().LoadFromFile("Arial Narrow Bold Italic", "arial_nbi.ttf");
    FontLoader::GetInstance().LoadFromFile("Bahnschrift Condensed Bold", "bahnschrift.ttf", 14);
    FontLoader::GetInstance().LoadFromFile("Cascadia Code Bold", "cascadia_code_bold.ttf");
    FontLoader::GetInstance().LoadFromFile("FFF Forward", "fff_forwa.ttf");
}

ResourceLoader* ResourceLoader::GetAppState()
{
    static ResourceLoader appState;
    return &appState;
}
