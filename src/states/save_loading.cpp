#include <states/save_loading.h>
#include <states/load_save.h>
#include <states/main_game.h>

#include <serialization/save_data.h>
#include <serialization/json_loader.h>
#include <util/directory_system.h>
#include <thread>

void SaveLoading::Init()
{
    // Initialize the member variables
    this->loadingProgress = 0.0f;
    this->opacity = 0.0f;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Start the save data loading thread
    std::thread savingThread(&SaveLoading::ExecuteLoadingProcess, this);
    savingThread.detach();
}

void SaveLoading::Destroy() {}

void SaveLoading::ExecuteLoadingProcess()
{
    // Get the metadata for the save we are loading
    const LoadSave::ExistingSave& saveMetadata = LoadSave::GetAppState()->GetSelectedExistingSave();

    // Store the save's metadata
    SaveData::GetInstance().SetSaveName(saveMetadata.fileName.substr(0, saveMetadata.fileName.size() - 5));
    SaveData::GetInstance().SetPlayerCount((uint8_t)saveMetadata.playerCount);
    SaveData::GetInstance().SetGrowthSystem((SaveData::GrowthSystemType)saveMetadata.growthSystemID);

    // Clear the data already in the player, club and user databases before loading
    SaveData::GetInstance().GetPlayerDatabase().clear();
    SaveData::GetInstance().GetClubDatabase().clear();
    SaveData::GetInstance().GetUsers().clear();

    // Now load the save data from the save file
    JSONLoader saveFileLoader(Util::GetAppDataDirectory() + "data/saves/" + saveMetadata.fileName);

    SaveData::GetInstance().LoadPlayersFromJSON(saveFileLoader.GetRoot(), false);

    {
        std::scoped_lock lock(this->mutex);
        this->loadingProgress = 50;
    }

    SaveData::GetInstance().LoadClubsFromJSON(saveFileLoader.GetRoot(), false);

    {
        std::scoped_lock lock(this->mutex);
        this->loadingProgress = 90;
    }

    SaveData::GetInstance().LoadUsersFromJSON(saveFileLoader.GetRoot());

    {
        std::scoped_lock lock(this->mutex);
        this->loadingProgress = 100;
    }

    saveFileLoader.Clear();
}

void SaveLoading::Update(const float& deltaTime)
{
    std::scoped_lock lock(this->mutex);

    // Once the loading process is complete, switch to the main game state
    if (this->loadingProgress == 100.0f)
        this->SwitchState(MainGame::GetAppState());
}

void SaveLoading::Render() const
{
    // Render the background
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 1"),
        { 255, 255, 255, this->opacity });

    // Render loading text
    Renderer::GetInstance().RenderText({ 425, 450 }, { 255, 255, 255, this->opacity }, FontLoader::GetInstance().GetFont("Bahnschrift Bold"), 75,
        "LOADING DATA, PLEASE WAIT...");

    // Render the progress bar edge
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1000, 100 }, { 255, 255, 255, 255 });

    // Render the progress bar
    std::scoped_lock lock(this->mutex);
    Renderer::GetInstance().RenderSquare({ 470 + (490 * (this->loadingProgress / 100)), 540 },
        { (990 * (this->loadingProgress / 100)), 90 }, { 128, 128, 128, 255 });
}

bool SaveLoading::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update fade in effect
    this->opacity = std::min(this->opacity + (transitionSpeed * deltaTime), 255.0f);

    if (this->opacity == 255.0f)
        return true;

    return false;
}

SaveLoading* SaveLoading::GetAppState()
{
    static SaveLoading appState;
    return &appState;
}
