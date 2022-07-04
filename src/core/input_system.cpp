#include <core/input_system.h>
#include <GLFW/glfw3.h>

namespace TextPolling
{
	static uint32_t characterInputted = 0x0;
	static void PollTextInputCallback(GLFWwindow* window, uint32_t codePoint)
	{
		characterInputted = codePoint;
	}
}

void InputSystem::Init(WindowFramePtr window)
{
	this->window = window;
	glfwSetCharCallback(window->GetFramePtr(), TextPolling::PollTextInputCallback);
}

bool InputSystem::WasKeyPressed(KeyCode key) const
{
	return glfwGetKey(this->window->GetFramePtr(), (int)key);
}

bool InputSystem::WasMouseButtonPressed(MouseCode button) const
{
	return glfwGetMouseButton(this->window->GetFramePtr(), (int)button);
}

glm::vec2 InputSystem::GetCursorPosition(const OrthogonalCamera* viewport) const
{
	// Get the current cursor position
	double cursorPosX = 0.0, cursorPosY = 0.0;
	glfwGetCursorPos(this->window->GetFramePtr(), &cursorPosX, &cursorPosY);

	// If a viewport camera was given then map the cursor position to it's dimensions
	if (viewport)
	{
		cursorPosX = cursorPosX * (viewport->GetSize().x / this->window->GetWidth());
		cursorPosY = cursorPosY * (viewport->GetSize().y / this->window->GetHeight());
	}

	return glm::vec2((float)cursorPosX, (float)cursorPosY);
}

uint32_t InputSystem::GetInputtedCharacter() const
{
	const uint32_t character = TextPolling::characterInputted;
	TextPolling::characterInputted = 0x0;

	return character;
}

InputSystem& InputSystem::GetInstance()
{
	static InputSystem instance;
	return instance;
}
