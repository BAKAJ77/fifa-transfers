#include <core/application_core.h>
#include <core/application_state.h>
#include <core/input_system.h>

#include <graphics/renderer.h>
#include <states/resource_loader.h>
#include <serialization/json_loader.h>

#include <util/directory_system.h>
#include <util/logging_system.h>
#include <util/timestamp.h>

#include <vector>


ApplicationCore::ApplicationCore()
{
	// Load the application config file (if it doesn't exist then generate a new one then load it)
	bool generateConfigSettings = false;
	if (!Util::IsExistingFile("config.json"))
		generateConfigSettings = true;

	JSONLoader configFile("config.json");
	if (generateConfigSettings)
	{
		configFile.GetRoot()["window"]["resolution"] = { 1600, 900 };
		configFile.GetRoot()["window"]["fullscreen"] = false;
		configFile.GetRoot()["window"]["vsync"] = false;

		configFile.GetRoot()["graphics"]["samplesMSAA"] = 4;
		configFile.GetRoot()["graphics"]["textResolutionLevel"] = 100;
		configFile.GetRoot()["graphics"]["gamma"] = 2.2f;
	}

	configFile.Close();

	const std::vector<int> configWindowResolution = configFile.GetRoot()["window"]["resolution"].get<std::vector<int>>();
	const bool configWindowFullscreen = configFile.GetRoot()["window"]["fullscreen"].get<bool>();
	const bool configWindowVsync = configFile.GetRoot()["window"]["vsync"].get<bool>();

	if (configWindowResolution.size() != 2)
		LogSystem::GetInstance().OutputLog("The window resolution config setting should only have 2 values, a width and height",
			Severity::FATAL);

	const uint32_t configSamplesPerPixel = configFile.GetRoot()["graphics"]["samplesMSAA"].get<uint32_t>();

	// Create the main application window
	this->window = Memory::CreateWindowFrame("FTFS 23", configWindowResolution[0], configWindowResolution[1], configWindowFullscreen,
		configWindowVsync, configSamplesPerPixel);

	// Initialize singleton systems
	InputSystem::GetInstance().Init(this->window);
	Renderer::GetInstance().Init(this->window);
	AppStateSystem::GetInstance().Init(this->window);

	AppStateSystem::GetInstance().SwitchState(ResourceLoader::GetAppState());

	// Continue onto the main loop
	this->MainLoop();
}

void ApplicationCore::MainLoop()
{
	Renderer::GetInstance().SetClearColor({ glm::vec3(70), 255 });

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