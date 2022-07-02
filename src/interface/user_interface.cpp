#include <interface/user_interface.h>
#include <util/logging_system.h>

UserInterface::UserInterface() :
    animationSpeed(8.0f), opacity(255.0f)
{}

UserInterface::UserInterface(WindowFramePtr window, float animationSpeed, float opacity) :
    appWindow(window), animationSpeed(animationSpeed), opacity(opacity)
{}

UserInterface::~UserInterface()
{
    // Deallocate all the standalone buttons
    for (auto iterator = this->standaloneButtons.begin(); iterator != this->standaloneButtons.end(); iterator++)
        delete *iterator;
}

void UserInterface::SetAnimationSpeed(float speed)
{
    this->animationSpeed = speed;
}

void UserInterface::SetOpacity(float opacity)
{
    this->opacity = opacity;
}

void UserInterface::AddStandaloneButton(ButtonBase* button)
{
    this->standaloneButtons.emplace_back(button);
}

void UserInterface::Update(const float& deltaTime)
{
    if (this->appWindow->IsFocused()) // Only update if the window is focused
    {
        // Update the standalone buttons
        for (ButtonBase* button : this->standaloneButtons)
            button->Update(deltaTime, this->animationSpeed);
    }
}

void UserInterface::Render() const
{
    // Render the standalone buttons
    for (const ButtonBase* button : this->standaloneButtons)
        button->Render(this->opacity);
}

const std::vector<ButtonBase*>& UserInterface::GetStandaloneButtons()
{
    return this->standaloneButtons;
}

const float& UserInterface::GetOpacity() const
{
    return this->opacity;
}
