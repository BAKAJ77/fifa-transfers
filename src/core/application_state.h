#ifndef APPLICATION_STATE_H
#define APPLICATION_STATE_H

#include <graphics/renderer.h>
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

	void SwitchState(AppState* appState);
	void PushState(AppState* appState);
	void PopState();
};

class AppStateSystem
{
private:
	std::vector<AppState*> stateStack;
private:
	AppStateSystem();
public:
	AppStateSystem(const AppStateSystem& other) = delete;
	AppStateSystem(AppStateSystem&& temp) noexcept = delete;
	~AppStateSystem() = default;

	AppStateSystem& operator=(const AppStateSystem& other) = delete;
	AppStateSystem& operator=(AppStateSystem&& temp) noexcept = delete;

	// Destroys all application states in the stack and starts up the new application state given.
	void SwitchState(AppState* appState);

	// Pauses the most recent application state (if any) and starts up the new application state given.
	void PushState(AppState* appState);

	// Destroys the last active application state in the stack.
	void PopState();

	// Updates the current active application state and other application states which are instructed to keep updated even when paused.
	void Update(const float& deltaTime);

	// Renders the current active application state and other application states which are instructed to keep rendering even when paused.
	void Render() const;

	// Returns TRUE if there is an active application state in the application state system stack.
	bool IsActive() const;

	static AppStateSystem& GetInstance();
};

#endif
