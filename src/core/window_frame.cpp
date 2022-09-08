#include <core/window_frame.h>
#include <core/application_core.h>
#include <util/logging_system.h>
#include <util/opengl_error.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

namespace WindowActivity
{
	bool isFocused = true;

	void WindowFocusCallback(GLFWwindow* window, int focused)
	{
		isFocused = (bool)focused;
	}
}

WindowFrame::WindowFrame(const std::string_view& title, int width, int height, bool fullscreen, bool vsync, uint32_t samplesPerPixel) :
	framePtr(nullptr), width(width), height(height), usingVsync(vsync), fullscreenEnabled(fullscreen),
	samplesPerPixel(std::max((int)samplesPerPixel, 1))
{
	// Initialize the GLFW library
	if (glfwInit() < 0)
		LogSystem::GetInstance().OutputLog("Failed to initialize GLFW", Severity::FATAL);

	// Setup GLFW window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, this->samplesPerPixel);
	glfwWindowHint(GLFW_RESIZABLE, false);
		
	// Create and setup the window
	fullscreen ? 
		this->framePtr = glfwCreateWindow(width, height, title.data(), glfwGetPrimaryMonitor(), nullptr) :
		this->framePtr = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

	if (!this->framePtr)
		LogSystem::GetInstance().OutputLog("An error occurred while creating the window", Severity::FATAL);

	const GLFWvidmode* monitorData = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(this->framePtr, (monitorData->width / 2) - (width / 2), (monitorData->height / 2) - (height / 2));
	glfwMakeContextCurrent(this->framePtr);
		
	if (vsync)
		glfwSwapInterval(1);

	glfwSetWindowFocusCallback(this->framePtr, WindowActivity::WindowFocusCallback);

	// Load the OpenGL function addresses
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		LogSystem::GetInstance().OutputLog("Failed to load OpenGL function addresses", Severity::FATAL);

	LogSystem::GetInstance().OutputLog((const char*)glGetString(GL_VERSION), Severity::INFO); // Log the version of OpenGL being used
		
	// Make sure to enable multisampling if specified to do so
	if (this->samplesPerPixel > 1)
		GLValidate(glEnable(GL_MULTISAMPLE));

	// Load the window icon and cursor
	GLFWimage iconImage, cursorImage;
	iconImage.pixels = stbi_load("misc/logo_48.png", &iconImage.width, &iconImage.height, nullptr, 0);
	cursorImage.pixels = stbi_load("textures/cursor.png", &cursorImage.width, &cursorImage.height, nullptr, 0);
	
	if (iconImage.pixels)
	{
		glfwSetWindowIcon(this->framePtr, 1, &iconImage);
		stbi_image_free(iconImage.pixels);

		if (cursorImage.pixels)
		{
			GLFWcursor* cursor = glfwCreateCursor(&cursorImage, 0, 0);
			glfwSetCursor(this->framePtr, cursor);
			stbi_image_free(cursorImage.pixels);
		}
		else
			LogSystem::GetInstance().OutputLog("Failed to load cursor image", Severity::WARNING);
	}
	else
		LogSystem::GetInstance().OutputLog("Failed to load application icon image", Severity::WARNING);


}

WindowFrame::~WindowFrame()
{
	glfwTerminate();
}

void WindowFrame::SetWidth(int width)
{
	glfwSetWindowSize(this->framePtr, width, this->height);
	this->width = width;
}

void WindowFrame::SetHeight(int height)
{
	glfwSetWindowSize(this->framePtr, this->width, height);
	this->height = height;
}

void WindowFrame::RequestClose() const
{
	glfwSetWindowShouldClose(this->framePtr, true);
}

void WindowFrame::Update() const
{
	glfwPollEvents();
	glfwSwapBuffers(this->framePtr);
}

bool WindowFrame::HasRequestedClose() const
{
	return glfwWindowShouldClose(this->framePtr);
}

GLFWwindow* WindowFrame::GetFramePtr()
{
	return this->framePtr;
}

const int& WindowFrame::GetWidth() const
{
	return this->width;
}

const int& WindowFrame::GetHeight() const
{
	return this->height;
}

const uint32_t& WindowFrame::GetSamplesPerPixel() const
{
	return this->samplesPerPixel;
}

bool WindowFrame::IsUsingVsyncMode() const
{
	return this->usingVsync;
}

bool WindowFrame::IsFullscreenEnabled() const
{
	return this->fullscreenEnabled;
}

bool WindowFrame::IsFocused() const
{
	return WindowActivity::isFocused;
}

WindowFramePtr Memory::CreateWindowFrame(const std::string_view& title, int width, int height, bool fullscreen, bool vsync, 
	uint32_t samplesPerPixel)
{
	return std::make_shared<WindowFrame>(title, width, height, fullscreen, vsync, samplesPerPixel);
}