#include <core/application_state.h>
#include <util/logging_system.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AppState::AppState() :
	renderWhilePaused(true), updateWhilePaused(false)
{}

void AppState::Resume() {}

void AppState::Pause() {}

bool AppState::OnStartupTransitionUpdate(const float deltaTime)
{
	return true;
}

bool AppState::OnPauseTransitionUpdate(const float deltaTime)
{
	return true;
}

bool AppState::OnResumeTransitionUpdate(const float deltaTime)
{
	return true;
}

void AppState::SwitchState(AppState* appState)
{
	AppStateSystem::GetInstance().SwitchState(appState);
}

void AppState::PushState(AppState* appState)
{
	AppStateSystem::GetInstance().PushState(appState);
}

void AppState::RollBack(AppState* appState)
{
	AppStateSystem::GetInstance().RollBack(appState);
}

void AppState::PopState()
{
	AppStateSystem::GetInstance().PopState();
}

WindowFramePtr AppState::GetAppWindow() const
{
	return AppStateSystem::GetInstance().GetAppWindow();
}

uint32_t AppState::GetStackSize() const
{
	return AppStateSystem::GetInstance().GetStackSize();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AppStateSystem::AppStateSystem() :
	currentCommand(SystemCommand::NONE), pendingAppState(nullptr)
{}

void AppStateSystem::Init(const WindowFramePtr window)
{
	this->appWindow = window;
}

void AppStateSystem::SwitchState(AppState* appState)
{
	this->currentCommand = SystemCommand::SWITCH;

	// Start the application state given
	appState->Init();
	this->pendingAppState = appState;
}

void AppStateSystem::PushState(AppState* appState)
{
	this->currentCommand = SystemCommand::PUSH;

	// Start the application state given
	appState->Init();
	this->pendingAppState = appState;
}

void AppStateSystem::RollBack(AppState* appState)
{
	// Make sure the given app state currently is in the stack
	bool foundInStack = false;
	for (const AppState* state : this->stateStack)
	{
		if (state == appState)
		{
			foundInStack = true;
			break;
		}
	}

	if (foundInStack)
	{
		// Keep popping states ahead in the stack until the app state given is reached
		for (auto it = this->stateStack.rbegin(); it != this->stateStack.rend(); it++)
		{
			if (*it == appState)
				break;

			this->stateStack.pop_back();
		}
	}
	else
		LogSystem::GetInstance().OutputLog("Rollback operation failure, state given is not in stack", Severity::WARNING);
}

void AppStateSystem::PopState()
{
	this->currentCommand = SystemCommand::POP;

	// Destroy the last active application state then pop if off the stack (if any)
	if (!this->stateStack.empty())
	{
		this->stateStack.back()->Destroy();
		this->stateStack.pop_back();
	}
}

void AppStateSystem::Update(const float& deltaTime)
{
	// Handle switch, push or pop operations
	switch (this->currentCommand)
	{
	case SystemCommand::SWITCH: // HANDLE SWITCH OPERATION
		if (this->pendingAppState->OnStartupTransitionUpdate(deltaTime))
		{
			// Destroy all the application states in the stack
			for (AppState* state : this->stateStack)
				state->Destroy();

			this->stateStack.clear();

			// Push the new active app state back into the stack
			this->stateStack.emplace_back(this->pendingAppState);

			// Reset current command and pending app state pointer
			this->currentCommand = SystemCommand::NONE;
			this->pendingAppState = nullptr;
		}
		break;
	case SystemCommand::POP: // HANDLE POP OPERATION
		if (!this->stateStack.empty())
		{
			if (this->stateStack.back()->OnResumeTransitionUpdate(deltaTime))
			{
				// Resume the application state at the top of the stack
				this->stateStack.back()->Resume();
				this->currentCommand = SystemCommand::NONE;
			}
		}
		break;
	case SystemCommand::PUSH: // HANDLE PUSH OPERATION
		static bool pauseTransitionComplete = false;

		if (!this->stateStack.empty() && !pauseTransitionComplete)
		{
			static bool calledPauseFunc = false;

			if (!calledPauseFunc) // This is to prevent the pause function from being called multiple times
			{
				this->stateStack.back()->Pause();
				calledPauseFunc = true;
			}

			// Pause the last active application state in the stack
			if ((pauseTransitionComplete = this->stateStack.back()->OnPauseTransitionUpdate(deltaTime)))
				calledPauseFunc = false;
		}
		else if (this->stateStack.empty()) // There are no other app states in the stack so no need for a pause transition
			pauseTransitionComplete = true;

		if (pauseTransitionComplete)
		{
			if (this->pendingAppState->OnStartupTransitionUpdate(deltaTime))
			{
				// Push the pending app state into the stack
				this->stateStack.emplace_back(this->pendingAppState);

				// Reset current command, pending app state pointer and the onPauseComplete boolean flag
				this->currentCommand = SystemCommand::NONE;
				this->pendingAppState = nullptr;

				pauseTransitionComplete = false;
			}
		}
		break;
	}

	// Update the current active application state (and application states which are set to be updated while paused)
	for (size_t stateIndex = 0; stateIndex < this->stateStack.size(); stateIndex++)
	{
		AppState* appState = this->stateStack[stateIndex];
		if (stateIndex == (this->stateStack.size() - 1) || appState->updateWhilePaused)
		{
			if (this->currentCommand != SystemCommand::SWITCH && this->currentCommand != SystemCommand::PUSH)
				appState->Update(deltaTime);
		}
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

	// Render the in-transition game state (if any)
	if (this->pendingAppState)
		this->pendingAppState->Render();
}

WindowFramePtr AppStateSystem::GetAppWindow() const
{
	return this->appWindow;
}

bool AppStateSystem::IsActive() const
{
	return !this->stateStack.empty() || this->pendingAppState;
}

uint32_t AppStateSystem::GetStackSize() const
{
	return (uint32_t)this->stateStack.size();
}

AppStateSystem& AppStateSystem::GetInstance()
{
	static AppStateSystem instance;
	return instance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////