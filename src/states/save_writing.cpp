#include <states/save_writing.h>
#include <states/new_save.h>

#include <serialization/save_data.h>
#include <util/logging_system.h>
#include <util/directory_system.h>
#include <util/random_engine.h>
#include <util/data_manip.h>

#include <thread>

void SaveWriting::Init()
{
    // Initialize the member variables
	this->shouldPopState = false;
    this->savingProgress = 0.0f;
	this->opacity = 0.0f;

    // Fetch the Bahnschrift Bold font
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Start the save data writting thread
    std::thread savingThread(&SaveWriting::ExecuteSavingProcess, this);
    savingThread.detach();
}

void SaveWriting::Destroy() {}

void SaveWriting::ExecuteSavingProcess()
{
    // Do some operations on the save data if it is a new save
    if (!Util::IsExistingFile(std::string("data/saves/") + SaveData::GetInstance().GetName().data() + ".json"))
    {
		for (UserProfile& user : SaveData::GetInstance().GetUsers())
		{
			// Generate the objectives for each user's club
			user.GetClub()->GenerateObjectives();

			// Set default training staff for each user's club
			user.GetClub()->GetTrainingStaff() = { { Club::StaffType::GOALKEEPING }, { Club::StaffType::DEFENCE }, { Club::StaffType::MIDFIELD }, 
				{ Club::StaffType::ATTACK } };
		}

		for (Club& club : SaveData::GetInstance().GetClubDatabase())
		{
			// Calculate the wage budget for every club
			float totalClubWages = 0;
			for (const Player* player : club.GetPlayers())
				totalClubWages += player->GetWage();

			club.SetWageBudget(Util::GetTruncatedSFInteger((int)(totalClubWages / 14.03306f), 3));
			club.SetInitialWageBudget(club.GetWageBudget());

			club.SetInitialTransferBudget(club.GetTransferBudget());

			// Randomise contract lengths of the players who's contracts are nearly up already
			for (Player* player : club.GetPlayers())
			{
				if (player->GetExpiryYear() <= SaveData::GetInstance().GetCurrentYear() + 1)
				{
					player->SetExpiryYear(player->GetExpiryYear() + RandomEngine::GetInstance().GenerateRandom<int>(0, 3) +
						(int)(player->GetExpiryYear() == SaveData::GetInstance().GetCurrentYear()));
				}
			}
		}

		// Randomise the potentials of player in the new save if specified to do so
		if (NewSave::GetAppState()->ShouldRandomisePotentials())
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

		// If the saves directory does not exist, create it
		if (!Util::IsExistingDirectory("data/saves"))
		{
			if (!Util::CreateNewDirectory("data/saves"))
				LogSystem::GetInstance().OutputLog("Failed to create the saves directory", Severity::FATAL);
		}
    }

	// Write the save data to the json file
    SaveData::GetInstance().Write(this->savingProgress, this->mutex);
}

void SaveWriting::Update(const float& deltaTime)
{
    std::scoped_lock lock(this->mutex);
    
    // Check if the save writting process is complete
	if (this->savingProgress == 100.0f)
	{
		if (this->nextAppState) // If an app state has been provided, then switch to that state
			this->SwitchState(this->nextAppState);
		else // No app state have been provided so just pop this state
			this->shouldPopState = true;
	}

	if (this->shouldPopState)
	{
		constexpr float transitionSpeed = 1000.0f;

		// Update fade out effect
		this->opacity = std::max(this->opacity - (transitionSpeed * deltaTime), 0.0f);

		if (this->opacity == 0.0f)
			this->PopState();
	}
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

void SaveWriting::SetNextState(AppState* appState)
{
	this->nextAppState = appState;
}
