#ifndef APPLICATION_STATE_H
#define APPLICATION_STATE_H

#include <core/audio_system.h>
#include <graphics/renderer.h>
#include <graphics/texture_loader.h>

#include <vector>

class AppState
{
	friend class AppStateSystem;
protected:
	OrthogonalCamera camera;
	bool updateWhilePaused, renderWhilePaused;
protected:
	AppState();
	virtual ~AppState() = default;

	// For initializing the application state e.g. loading textures.
	virtual void Init() = 0;

	// For destroying the application state e.g. cleaning up resources used.
	virtual void Destroy() = 0;

	// For acting upon when the application state is being resumed.
	virtual void Resume();

	// For acting upon when the application state is being paused.
	virtual void Pause();

	// For updating the application state.
	virtual void Update(const float& deltaTime) = 0;

	// For rendering objects in the application state.
	virtual void Render() const = 0;

	// For handling the transition of the new app state being started.
	// This method should return TRUE when the transition is finished.
	virtual bool OnStartupTransitionUpdate(const float deltaTime);

	// For handling the transition of a existing app state being paused.
	// This method should return TRUE when the transition is finished.
	virtual bool OnPauseTransitionUpdate(const float deltaTime);

	// For handling the transition of a existing app state being resumed.
	// This method should return TRUE when the transition is finished.
	virtual bool OnResumeTransitionUpdate(const float deltaTime);
	
	void SwitchState(AppState* appState);
	void PushState(AppState* appState);
	void RollBack(AppState* appState);
	void PopState();

	WindowFramePtr GetAppWindow() const;
	uint32_t GetStackSize() const;
};

class AppStateSystem
{
	enum class SystemCommand
	{
		NONE,
		SWITCH,
		PUSH,
		POP
	};
private:
	WindowFramePtr appWindow;

	std::vector<AppState*> stateStack;
	SystemCommand currentCommand;
	AppState* pendingAppState;
private:
	AppStateSystem();
public:
	AppStateSystem(const AppStateSystem& other) = delete;
	AppStateSystem(AppStateSystem&& temp) noexcept = delete;
	~AppStateSystem() = default;

	AppStateSystem& operator=(const AppStateSystem& other) = delete;
	AppStateSystem& operator=(AppStateSystem&& temp) noexcept = delete;

	// Initializes the application state system.
	void Init(const WindowFramePtr window);

	// Destroys all application states in the stack and starts up the new application state given.
	void SwitchState(AppState* appState);

	// Pauses the most recent application state (if any) and starts up the new application state given.
	void PushState(AppState* appState);

	// Pops all app states in the stack which are ahead of the app state given.
	// Note that if the app state given isn't in the stack then no app states will be popped.
	void RollBack(AppState* appState);

	// Destroys the last active application state in the stack.
	void PopState();

	// Updates the current active application state and other application states which are instructed to keep updated even when paused.
	void Update(const float& deltaTime);

	// Renders the current active application state and other application states which are instructed to keep rendering even when paused.
	void Render() const;

	// Returns the application window.
	WindowFramePtr GetAppWindow() const;

	// Returns TRUE if there is an active application state in the application state system stack.
	bool IsActive() const;

	// Returns the amount of app states in the stack.
	uint32_t GetStackSize() const;

	// Returns singleton instance object of this class.
	static AppStateSystem& GetInstance();
};

#endif
