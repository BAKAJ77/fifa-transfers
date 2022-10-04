#ifndef APPLICATION_CORE_H
#define APPLICATION_CORE_H

#include <core/window_frame.h>

class ApplicationCore
{
private:
	WindowFramePtr window;
private:
	// The main loop of the application is executed here.
	void MainLoop();

	// Updating the application logic is handled here.
	void Update(const float& deltaTime);

	// Rendering to the screen is handled here.
	void Render() const;
public:
	ApplicationCore();
	~ApplicationCore();
};

#endif