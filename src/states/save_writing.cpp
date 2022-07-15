#include <states/save_writing.h>
#include <states/new_save.h>
#include <states/main_game.h>

#include <serialization/save_data.h>

#include <util/directory_system.h>
#include <util/random_engine.h>
#include <util/data_manip.h>

#include <thread>

void SaveWriting::Init()
{
    // Initialize the member variables
    this->savingProgress = 0.0f;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Start the save data writting thread
    std::thread savingThread(&SaveWriting::ExecuteSavingProcess, this);
    savingThread.detach();
}

void SaveWriting::Destroy() {}

void SaveWriting::ExecuteSavingProcess()
{
    // Before writing the save data, do the potential randomisation process if:
    // The user specified to and if this is a new save we are writing
    if (NewSave::GetAppState()->ShouldRandomisePotentials() &&
        !Util::IsExistingFile(Util::GetAppDataDirectory() + "data/saves/" + SaveData::GetInstance().GetName().data() + ".json"))
    {
		constexpr float noIncreaseThreshold = 0.225f;
		int noPotentialIncreaseCount = 0, numNonPrimeAgePlayers = 0;
		bool generationRerun = false;

		do
		{
			for (auto& player : SaveData::GetInstance().GetPlayerDatabase()) // Loop through each player in the database
			{
				constexpr int primeAge = 27;
				const int primeAgeDifference = std::max(primeAge - player.GetAge(), 0);

				if ((generationRerun && (player.GetPotential() - player.GetOverall() == 0)) || !generationRerun)
				{
					// Calculate the player's potential based on their age and current rating
					constexpr int maxIncrease = 25;
					int increasePotential = 0, generationCount = 0;

					do
					{
						const int potentialMultiplier = RandomEngine::GetInstance().GenerateRandom<int>(0, primeAgeDifference);
						increasePotential = potentialMultiplier * RandomEngine::GetInstance().GenerateRandom<int>(1, 4);
					} while (player.GetOverall() + increasePotential > 94 && generationCount < 3);

					if (increasePotential == 0 && primeAgeDifference > 0)
						noPotentialIncreaseCount++;

					player.SetPotential(std::min(player.GetOverall() + std::min(increasePotential, maxIncrease), 99));

					// Calculate value increase based on generated potential
					if (increasePotential > 0)
					{
						float value_multiplier = 0.0f;
						if (player.GetOverall() <= 70)
							value_multiplier = ((float)player.GetPotential() / (float)player.GetOverall()) * 2.5f;
						else
							value_multiplier = ((float)player.GetPotential() / (float)player.GetOverall());

						player.SetValue(Util::GetTruncatedSFInteger((int)(player.GetValue() * value_multiplier), 3));
						if (player.GetReleaseClause() > 0)
							player.SetReleaseClause(Util::GetTruncatedSFInteger((int)(player.GetReleaseClause() * value_multiplier), 4));
					}
				}

				if (primeAgeDifference > 0)
					numNonPrimeAgePlayers++;
			}

			((float)noPotentialIncreaseCount / (float)numNonPrimeAgePlayers) >= noIncreaseThreshold ? 
				generationRerun = true : generationRerun = false;

			noPotentialIncreaseCount = 0;
			numNonPrimeAgePlayers = 0;
		} while (generationRerun);
    }

	// Write the save data to the json file
    SaveData::GetInstance().Write(this->savingProgress, this->mutex);
}

void SaveWriting::Update(const float& deltaTime)
{
    std::scoped_lock lock(this->mutex);
    
    // Once the saving process is complete, switch to the main game state
    if (this->savingProgress == 100.0f)
        this->SwitchState(MainGame::GetAppState());
}

void SaveWriting::Render() const
{
    // Render the background
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1920, 1080 }, TextureLoader::GetInstance().GetTexture("Background 1"),
        { 255, 255, 255, this->opacity });

    // Render loading text
    Renderer::GetInstance().RenderText({ 450, 450 }, { 255, 255, 255, this->opacity }, FontLoader::GetInstance().GetFont("Bahnschrift Bold"), 75,
        "SAVING DATA, PLEASE WAIT...");

    // Render the progress bar edge
    Renderer::GetInstance().RenderSquare({ 960, 540 }, { 1000, 100 }, { 255, 255, 255, 255 });

    // Render the progress bar
    std::scoped_lock lock(this->mutex);
    Renderer::GetInstance().RenderSquare({ 470 + (490 * (this->savingProgress / 100)), 540 },
        { (990 * (this->savingProgress / 100)), 90 }, { 128, 128, 128, 255 });
}

bool SaveWriting::OnStartupTransitionUpdate(const float deltaTime)
{
    constexpr float transitionSpeed = 1000.0f;

    // Update fade in effect
    this->opacity = std::min(this->opacity + (transitionSpeed * deltaTime), 255.0f);

    if (this->opacity == 255.0f)
        return true;

    return false;
}

SaveWriting* SaveWriting::GetAppState()
{
    static SaveWriting appState;
    return &appState;
}
