#include <core/window_frame.h>
#include <core/application_core.h>
#include <util/logging_system.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

WindowFrame::WindowFrame(const std::string_view& title, int width, int height, bool fullscreen, bool vsync, ApplicationCore& appCore) :
	framePtr(nullptr), width(width), height(height), usingVsync(vsync), fullscreenEnabled(fullscreen), appCore(appCore)
{
	if (appCore.IsGLFWInitialized())
	{
		// Setup GLFW window hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

		// Load the OpenGL function addresses
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			LogSystem::GetInstance().OutputLog("Failed to load OpenGL function addresses", Severity::FATAL);

		LogSystem::GetInstance().OutputLog((const char*)glGetString(GL_VERSION), Severity::INFO); // Log the version of OpenGL being used
	}
	else
		LogSystem::GetInstance().OutputLog("Skipped window creation as GLFW is not initialized", Severity::WARNING);
}

WindowFrame::~WindowFrame()
{
	// GLFW windows are automatically destroyed when glfwTerminate() is called,
	// also calling glfwDestroyWindow() when GLFW is not initialized will cause an exception to occur
	if (this->appCore.IsGLFWInitialized())
		glfwDestroyWindow(this->framePtr);
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

int WindowFrame::GetWidth() const
{
	return this->width;
}

int WindowFrame::GetHeight() const
{
	return this->height;
}

bool WindowFrame::IsUsingVsyncMode() const
{
	return this->usingVsync;
}

bool WindowFrame::IsFullscreenEnabled() const
{
	return this->fullscreenEnabled;
}

WindowFramePtr Memory::CreateWindowFrame(const std::string_view& title, int width, int height, bool fullscreen, bool vsync, 
	ApplicationCore& appCore)
{
	return std::make_shared<WindowFrame>(title, width, height, fullscreen, vsync, appCore);
}
