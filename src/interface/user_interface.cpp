#include <interface/user_interface.h>
#include <core/input_system.h>
#include <util/logging_system.h>
#include <util/timestamp.h>

UserInterface::UserInterface() :
    animationSpeed(8.0f), opacity(255.0f)
{}

UserInterface::UserInterface(WindowFramePtr window, float animationSpeed, float opacity) :
    appWindow(window), animationSpeed(animationSpeed), opacity(opacity)
{}

UserInterface::~UserInterface()
{
    // Deallocate all the standalone buttons
    for (auto iterator = this->buttons.begin(); iterator != this->buttons.end(); iterator++)
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

void UserInterface::AddButton(ButtonBase* button)
{
    this->buttons.emplace_back(button);
}

void UserInterface::AddTextField(const std::string_view& id, const TextInputField& field)
{
    // Don't add the text field to the interface if another with the ID given already exists
    for (const auto& field : this->textFields)
    {
        if (field.first == id.data())
        {
            LogSystem::GetInstance().OutputLog("There's already a text field with the ID: " + std::string(id), Severity::WARNING);
            return;
        }
    }

    this->textFields.push_back({ id.data(), field });
}

void UserInterface::AddRadioButtonGroup(const std::string_view& id, const RadioButtonGroup& group)
{
    // Don't add the radio button group to the interface if another with the ID given already exists
    if (this->radioButtonGroups.find(id.data()) != this->radioButtonGroups.end())
    {
        LogSystem::GetInstance().OutputLog("There's already a radio button group with the ID: " + std::string(id), Severity::WARNING);
        return;
    }

    this->radioButtonGroups[id.data()] = group;
}

void UserInterface::AddDropDown(const std::string_view& id, const DropDown& dropDown)
{
    // Don't add the drop down to the interface if another with the ID given already exists
    if (this->dropDowns.find(id.data()) != this->dropDowns.end())
    {
        LogSystem::GetInstance().OutputLog("There's already a drop down with the ID: " + std::string(id), Severity::WARNING);
        return;
    }

    this->dropDowns[id.data()] = dropDown;
}

void UserInterface::AddSelectionList(const std::string_view& id, const SelectionList& list)
{
    // Don't add the selection list to the interface if another with the ID given already exists
    if (this->selectionLists.find(id.data()) != this->selectionLists.end())
    {
        LogSystem::GetInstance().OutputLog("There's already a selection list with the ID: " + std::string(id), Severity::WARNING);
        return;
    }

    this->selectionLists[id.data()] = list;
}

void UserInterface::AddTickBox(const std::string_view& id, const TickBox& tickBox)
{
    // Don't add the tick box to the interface if another with the ID given already exists
    if (this->tickBoxes.find(id.data()) != this->tickBoxes.end())
    {
        LogSystem::GetInstance().OutputLog("There's already a tick box with the ID: " + std::string(id), Severity::WARNING);
        return;
    }

    this->tickBoxes[id.data()] = tickBox;
}

void UserInterface::Update(const float& deltaTime)
{
    if (this->appWindow->IsFocused()) // Only update if the window is focused
    {
        // Drop down elements have priority focus, therefore if a drop down is active (aka dropped down) 
        // then only update the active drop down and skip updating every other element.
        DropDown* activeDropDown = nullptr;
        for (auto& dropDown : this->dropDowns)
        {
            if (dropDown.second.IsDroppedDown())
            {
                activeDropDown = &dropDown.second;
                break;
            }
        }

        if (activeDropDown)
            activeDropDown->Update(deltaTime); // Update the active drop down only
        else
        {
            // If TAB is pressed, then switch focus to next text input field
            static float previousTime = Util::GetSecondsSinceEpoch();
            if (InputSystem::GetInstance().WasKeyPressed(KeyCode::KEY_TAB) && Util::GetSecondsSinceEpoch() - previousTime >= 0.2f)
            {
                bool isFocusedOnTextField = false;
                for (size_t i = 0; i < this->textFields.size(); i++)
                {
                    if (this->textFields[i].second.IsFocused())
                    {
                        this->textFields[(i + 1) % this->textFields.size()].second.SetFocus(true);
                        isFocusedOnTextField = true;
                        break;
                    }
                }

                if (!isFocusedOnTextField)
                    this->textFields[0].second.SetFocus(true);

                previousTime = Util::GetSecondsSinceEpoch();
            }

            // Update the drop downs
            for (auto& dropDown : this->dropDowns)
                dropDown.second.Update(deltaTime);

            // Update the standalone buttons
            for (auto& button : this->buttons)
                button->Update(deltaTime, this->animationSpeed);

            // Update the standalone text fields
            for (auto& field : this->textFields)
                field.second.Update(deltaTime);

            // Update the radio button groups
            for (auto& group : this->radioButtonGroups)
                group.second.Update(deltaTime);

            // Update the selection lists
            for (auto& list : this->selectionLists)
                list.second.Update(deltaTime);

            // Update the tick boxes
            for (auto& tickBox : this->tickBoxes)
                tickBox.second.Update(deltaTime, this->animationSpeed);
        }
    }
}

void UserInterface::Render() const
{
    if (this->opacity > 0.0f)
    {
        // Render the standalone buttons
        for (const ButtonBase* button : this->buttons)
            button->Render(this->opacity);

        // Render the standalone text fields
        for (const auto& field : this->textFields)
            field.second.Render(this->opacity);

        // Render the radio button groups
        for (auto& group : this->radioButtonGroups)
            group.second.Render(this->opacity);

        // Render the selection lists
        for (auto& list : this->selectionLists)
            list.second.Render(this->opacity);

        // Render the tick boxes
        for (auto& tickBox : this->tickBoxes)
            tickBox.second.Render(this->opacity);

        // Render the drop downs
        const DropDown* activeDropDown = nullptr;
        for (auto& dropDown : this->dropDowns)
            dropDown.second.IsDroppedDown() ? activeDropDown = &dropDown.second : dropDown.second.Render(this->opacity);

        if (activeDropDown) // This is to make sure the active drop down renders over other inactive drop downs
            activeDropDown->Render(this->opacity);
    }
}

TextInputField* UserInterface::GetTextField(const std::string_view& id)
{
    for (auto& field : this->textFields)
    {
        if (field.first == id.data())
            return &field.second;
    }

    // No text field has been found matching the ID given
    LogSystem::GetInstance().OutputLog("No text field exists with the ID: " + std::string(id), Severity::WARNING);
    return nullptr;
}

const std::vector<ButtonBase*>& UserInterface::GetButtons()
{
    return this->buttons;
}

RadioButtonGroup* UserInterface::GetRadioButtonGroup(const std::string_view& id)
{
    auto iterator = this->radioButtonGroups.find(id.data());
    if (iterator != this->radioButtonGroups.end())
        return &iterator->second;

    // No radio button group has been found matching the ID given
    LogSystem::GetInstance().OutputLog("No radio button group exists with the ID: " + std::string(id), Severity::WARNING);
    return nullptr;
}

DropDown* UserInterface::GetDropDown(const std::string_view& id)
{
    auto iterator = this->dropDowns.find(id.data());
    if (iterator != this->dropDowns.end())
        return &iterator->second;

    // No drop down has been found matching the ID given
    LogSystem::GetInstance().OutputLog("No drop down exists with the ID: " + std::string(id), Severity::WARNING);
    return nullptr;
}

SelectionList* UserInterface::GetSelectionList(const std::string_view& id)
{
    auto iterator = this->selectionLists.find(id.data());
    if (iterator != this->selectionLists.end())
        return &iterator->second;

    // No selection list has been found matching the ID given
    LogSystem::GetInstance().OutputLog("No selection list exists with the ID: " + std::string(id), Severity::WARNING);
    return nullptr;
}

TickBox* UserInterface::GetTickBox(const std::string_view& id)
{
    auto iterator = this->tickBoxes.find(id.data());
    if (iterator != this->tickBoxes.end())
        return &iterator->second;

    // No tick box has been found matching the ID given
    LogSystem::GetInstance().OutputLog("No tick box exists with the ID: " + std::string(id), Severity::WARNING);
    return nullptr;
}

const float& UserInterface::GetOpacity() const
{
    return this->opacity;
}
