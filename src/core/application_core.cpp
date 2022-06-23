#include <core/application_core.h>
#include <serialization/config.h>
#include <util/directory_system.h>
#include <util/logging_system.h>
#include <util/timestamp.h>

#include <GLFW/glfw3.h>
#include <vector>

ApplicationCore::ApplicationCore() :
	initializedGLFW(false)
{
	// Initialize the GLFW library module
	if (glfwInit() < 0)
		LogSystem::GetInstance().OutputLog("Failed to initialize GLFW", Severity::FATAL);
	else
		this->initializedGLFW = true;

	// Load the app config file (if it doesn't exist then generate a new one then load it)
	if (!Util::IsExistingFile(Util::GetAppDataDirectory() + "config.json"))
		Serialization::GenerateConfigFile();

	const std::vector<int> configWindowResolution = Serialization::GetConfigElement<std::vector<int>>("window", "resolution");
	const bool configWindowFullscreen = Serialization::GetConfigElement<bool>("window", "fullscreen");
	const bool configWindowVsync = Serialization::GetConfigElement<bool>("window", "vsync");

	// Create the main application window
	this->window = Memory::CreateWindowFrame("FTFS 23", configWindowResolution[0], configWindowResolution[1], configWindowFullscreen,
		configWindowVsync, *this);

	// Continue onto the main loop
	this->MainLoop();
}

ApplicationCore::~ApplicationCore()
{
	// Clean up and terminate the GLFW library module
	glfwTerminate();
	this->initializedGLFW = false;
}

void ApplicationCore::MainLoop()
{
	constexpr float timeStep = 0.001f;
	float accumulatedRenderTime = 0.0f, elapsedRenderTime = 0.0f;

	while (!this->window->HasRequestedClose())
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
}

void ApplicationCore::Render() const
{
}

bool ApplicationCore::IsGLFWInitialized() const
{
	return this->initializedGLFW;
}