#include <core/application_core.h>
#include <core/application_state.h>
#include <core/input_system.h>

#include <graphics/renderer.h>
#include <states/splash_screen.h>
#include <serialization/config.h>

#include <util/directory_system.h>
#include <util/logging_system.h>
#include <util/timestamp.h>

#include <vector>


ApplicationCore::ApplicationCore()
{
	// Load the application config file (if it doesn't exist then generate a new one then load it)
	bool generateConfigSettings = false;
	if (!Util::IsExistingFile(Util::GetAppDataDirectory() + "config.json"))
		generateConfigSettings = true;

	ConfigLoader mainConfigFile(Util::GetAppDataDirectory() + "config.json");
	if (generateConfigSettings)
	{
		mainConfigFile.SetElement<std::vector<uint32_t>>("resolution", { 1600, 900 }, "window");
		mainConfigFile.SetElement<bool>("fullscreen", false, "window");
		mainConfigFile.SetElement<bool>("vsync", false, "window");

		mainConfigFile.SetElement<uint32_t>("samplesMSAA", 4, "graphics");
		mainConfigFile.SetElement<uint32_t>("textQualityLevel", 100, "graphics");
		mainConfigFile.SetElement<float>("gamma", 2.2f, "graphics");
	}

	mainConfigFile.Close();

	const std::vector<int> configWindowResolution = mainConfigFile.GetElement<std::vector<int>>("resolution", "window");
	const bool configWindowFullscreen = mainConfigFile.GetElement<bool>("fullscreen", "window");
	const bool configWindowVsync = mainConfigFile.GetElement<bool>("vsync", "window");

	if (configWindowResolution.size() != 2)
		LogSystem::GetInstance().OutputLog("The window resolution json config setting should only have 2 values, a width and height",
			Severity::FATAL);

	const uint32_t configSamplesPerPixel = mainConfigFile.GetElement<uint32_t>("samplesMSAA", "graphics");

	// Create the main application window
	this->window = Memory::CreateWindowFrame("FTFS 23", configWindowResolution[0], configWindowResolution[1], configWindowFullscreen,
		configWindowVsync, configSamplesPerPixel);

	// Initialize singleton systems
	InputSystem::GetInstance().Init(this->window);
	Renderer::GetInstance().Init(this->window);

	AppStateSystem::GetInstance().SwitchState(SplashScreen::GetAppState());

	// Continue onto the main loop
	this->MainLoop();
}

void ApplicationCore::MainLoop()
{
	constexpr float timeStep = 0.001f;
	float accumulatedRenderTime = 0.0f, elapsedRenderTime = 0.0f;

	while (!this->window->HasRequestedClose() && AppStateSystem::GetInstance().IsActive())
	{
		// Update the application logic
		accumulatedRenderTime += elapsedRenderTime;
		while (accumulatedRenderTime >= timeStep)
		{
			this->Update(timeStep);
			accumulatedRenderTime -= timeStep;
		}

		// Render to screen and calculate the amount time taken to render
		const float preRenderTime = Util::GetSecondsSinceEpoch();

		this->Render();
		this->window->Update();

		const float postRenderTime = Util::GetSecondsSinceEpoch();
		elapsedRenderTime = postRenderTime - preRenderTime;
	}
}

void ApplicationCore::Update(const float& deltaTime)
{
	AppStateSystem::GetInstance().Update(deltaTime);
}

void ApplicationCore::Render() const
{
	AppStateSystem::GetInstance().Render();
}