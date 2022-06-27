#ifndef WINDOW_FRAME_H
#define WINDOW_FRAME_H

#include <string_view>
#include <memory>

class ApplicationCore;
struct GLFWwindow;

class WindowFrame
{
private:
	GLFWwindow* framePtr;

	int width, height;
	uint32_t samplesPerPixel;
	bool usingVsync, fullscreenEnabled;
public:
	WindowFrame(const std::string_view& title, int width, int height, bool fullscreen, bool vsync, uint32_t samplesPerPixel);
	~WindowFrame();

	// Sets the width of the window.
	void SetWidth(int width);

	// Sets the height of the window.
	void SetHeight(int height);

	// Requests for the window to be close.
	void RequestClose() const;

	// Swaps window buffers and polls for window events.
	void Update() const;

	// Returns TRUE if the window has been requested to close.
	bool HasRequestedClose() const;

	// Returns pointer to the generated GLFWwindow struct.
	GLFWwindow* GetFramePtr();

	// Returns the width of the window.
	const int& GetWidth() const;

	// Returns the height of the window.
	const int& GetHeight() const;

	// Returns the amount of samples per pixel being used.
	// Anything above 1 indicates that MSAA is being used.
	const uint32_t& GetSamplesPerPixel() const;

	// Returns TRUE if the window is using vsync.
	bool IsUsingVsyncMode() const;

	// Returns TRUE if the window is in fullscreen mode.
	bool IsFullscreenEnabled() const;
};

using WindowFramePtr = std::shared_ptr<WindowFrame>;
namespace Memory
{
	// Creates a new window and returns shared pointer to the created window object.
	WindowFramePtr CreateWindowFrame(const std::string_view& title, int width, int height, bool fullscreen, bool vsync, 
		uint32_t samplesPerPixel);
}

#endif