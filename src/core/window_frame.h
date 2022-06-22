#ifndef WINDOW_FRAME_H
#define WINDOW_FRAME_H

#include <string_view>
#include <memory>

class ApplicationCore;
struct GLFWwindow;

class WindowFrame
{
private:
	ApplicationCore& appCore;
	GLFWwindow* framePtr;
	int width, height;
public:
	WindowFrame(const std::string_view& title, int width, int height, ApplicationCore& appCore);
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
	int GetWidth() const;

	// Returns the height of the window.
	int GetHeight() const;
};

using WindowFramePtr = std::shared_ptr<WindowFrame>;
namespace Memory
{
	// Creates a new window and returns shared pointer to the created window object.
	WindowFramePtr CreateWindowFrame(const std::string_view& title, int width, int height, ApplicationCore& appCore);
}

#endif