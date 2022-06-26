#include <core/application_state.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AppState::AppState() :
	renderWhilePaused(true), updateWhilePaused(false)
{}

void AppState::Resume() {}

void AppState::Pause() {}

void AppState::SwitchState(AppState* appState)
{
	AppStateSystem::GetInstance().SwitchState(appState);
}

void AppState::PushState(AppState* appState)
{
	AppStateSystem::GetInstance().PushState(appState);
}

void AppState::PopState()
{
	AppStateSystem::GetInstance().PopState();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AppStateSystem::AppStateSystem() {}

void AppStateSystem::SwitchState(AppState* appState)
{
	// Destroy all the application states in the stack
	for (AppState* state : this->stateStack)
		state->Destroy();

	this->stateStack.clear();

	// Start the application state given then push into stack
	appState->Init();
	this->stateStack.emplace_back(appState);
}

void AppStateSystem::PushState(AppState* appState)
{
	// Pause the last active application state in the stack (if any)
	if (!this->stateStack.empty())
		this->stateStack.back()->Pause();

	// Start the application state given then push into stack
	appState->Init();
	this->stateStack.emplace_back(appState);
}

void AppStateSystem::PopState()
{
	// Destroy the last active application state then pop if off the stack (if any)
	if (!this->stateStack.empty())
	{
		this->stateStack.back()->Destroy();
		this->stateStack.pop_back();
	}

	// Resume the application state at the top of the stack (if any)
	if (!this->stateStack.empty())
		this->stateStack.back()->Resume();
}

void AppStateSystem::Update(const float& deltaTime)
{
	// Update the current active application state (and application states which are set to be updated while paused)
	for (size_t stateIndex = 0; stateIndex < this->stateStack.size(); stateIndex++)
	{
		AppState* appState = this->stateStack[stateIndex];
		if (stateIndex == (this->stateStack.size() - 1) || appState->updateWhilePaused)
			appState->Update(deltaTime);
	}
}

void AppStateSystem::Render() const
{
	Renderer::GetInstance().Clear();

	// Render the current active application state (and application states which are set to be render while paused)
	for (size_t stateIndex = 0; stateIndex < this->stateStack.size(); stateIndex++)
	{
		const AppState* appState = this->stateStack[stateIndex];
		if (stateIndex == (this->stateStack.size() - 1) || appState->renderWhilePaused)
			appState->Render();
	}

	Renderer::GetInstance().Flush();
}

bool AppStateSystem::IsActive() const
{
	return !this->stateStack.empty();
}

AppStateSystem& AppStateSystem::GetInstance()
{
	static AppStateSystem instance;
	return instance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////