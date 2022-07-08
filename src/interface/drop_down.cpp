#include <interface/drop_down.h>
#include <core/input_system.h>
#include <graphics/renderer.h>
#include <util/logging_system.h>

DropDown::DropDown() :
    opacity(0.0f), doDropDown(false), fontSize(0), clicked(false), released(false)
{}

DropDown::DropDown(const glm::vec2& pos, const glm::vec2& size, uint16_t maxSelections, float opacity) :
    position(pos), size(size), opacity(opacity), doDropDown(false), clicked(false), released(false), fontSize(size.y / 2.5f)
{
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    this->currentSelected = { std::string(), 
        { pos, size, size, glm::vec3(60), glm::vec3(90), glm::vec3(120), this->opacity, 2.5f }, glm::vec2(0), -1}; // Set empty selection as default

    this->selections.reserve(maxSelections);
}

DropDown::DropDown(DropDown&& temp) noexcept :
    font(std::move(temp.font)), position(std::move(temp.position)), size(std::move(temp.size)), fontSize(std::move(temp.fontSize)), 
    opacity(std::move(temp.opacity)), selections(std::move(temp.selections)), currentSelected(std::move(temp.currentSelected)), clicked(false), 
    released(false), doDropDown(false)
{}

DropDown& DropDown::operator=(DropDown&& temp) noexcept
{
    this->font = std::move(temp.font);
    this->position = std::move(temp.position);
    this->size = std::move(temp.size);
    this->fontSize = std::move(temp.fontSize);
    this->opacity = std::move(temp.opacity);
    
    this->selections = std::move(temp.selections);
    this->currentSelected = std::move(temp.currentSelected);

    return *this;
}

void DropDown::SetPosition(const glm::vec2& pos)
{
    this->position = pos;
}

void DropDown::SetSize(const glm::vec2& size)
{
    this->size = size;
}

void DropDown::SetOpacity(float opacity)
{
    this->opacity = opacity;
}

void DropDown::AddSelection(const std::string_view& id, int value)
{
    if (this->selections.size() < this->selections.capacity())
    {
        this->selections.push_back({ id.data(), { { this->position.x, this->position.y + ((float)(this->selections.size() + 1) * this->size.y) },
            this->size, this->size, glm::vec3(85), glm::vec3(115), glm::vec3(85), this->opacity, 2.5f },
            Renderer::GetInstance().GetTextSize(this->font, (uint32_t)this->fontSize, id), value });
    }
    else
        LogSystem::GetInstance().OutputLog("Couldn't add new drop down element, the maximum has already been reached", Severity::WARNING);
}

void DropDown::Update(const float& deltaTime)
{
    // Update the clicked and released boolean flags
    if (InputSystem::GetInstance().WasMouseButtonPressed(MouseCode::MOUSE_BUTTON_LEFT) && this->released)
    {
        this->clicked = true;
        this->released = false;
    }
    else
        this->clicked = false;

    if (!InputSystem::GetInstance().WasMouseButtonPressed(MouseCode::MOUSE_BUTTON_LEFT) && !this->released)
        this->released = true;

    // Update the selection element buttons if the drop down has been activated
    if (this->doDropDown)
    {
        bool buttonClicked = false;
        for (size_t i = 0; i < this->selections.size(); i++)
        {
            this->selections[i].button.Update(deltaTime, 8.0f);

            if (this->selections[i].button.WasClicked())
            {
                this->currentSelected.id = this->selections[i].id;
                this->currentSelected.textSize = this->selections[i].textSize;
                this->currentSelected.value = this->selections[i].value;

                this->doDropDown = false;
                buttonClicked = true;
                break;
            }
        }

        if (this->clicked && !buttonClicked)
            this->doDropDown = false; // The drop down has lost focus so deactivate it
    }

    // Update the current selection button
    this->currentSelected.button.Update(deltaTime, 8.0f);
    if (this->currentSelected.button.WasClicked())
        this->doDropDown = true;
}

void DropDown::Render(float masterOpacity) const
{
    constexpr float offset = 7.5f;

    // Render the current selection button
    this->currentSelected.button.Render(masterOpacity);

    // Render the current selection button text
    Renderer::GetInstance().RenderText({ this->position.x - (this->currentSelected.textSize.x / 2) + offset,
        this->position.y + (this->currentSelected.textSize.y / 2) }, glm::vec4(255, 255, 255, (this->opacity * masterOpacity) / 255.0f), this->font,
        (uint32_t)this->fontSize, this->currentSelected.id);

    if (this->doDropDown)
    {
        for (size_t i = 0; i < this->selections.size(); i++)
        {
            // Render the selection element button body
            this->selections[i].button.Render(masterOpacity);

            // Render the selection element button text
            Renderer::GetInstance().RenderText({ this->selections[i].button.GetPosition().x - (this->selections[i].textSize.x / 2) + offset,
                this->selections[i].button.GetPosition().y + (this->selections[i].textSize.y / 2) }, 
                glm::vec4(255, 255, 255, (this->opacity * masterOpacity) / 255.0f), this->font, (uint32_t)this->fontSize, this->selections[i].id);
        }
    }
}

int DropDown::GetCurrentSelected() const
{
    return this->currentSelected.value;
}

const glm::vec2& DropDown::GetPosition() const
{
    return this->position;
}

const glm::vec2& DropDown::GetSize() const
{
    return this->size;
}

const float& DropDown::GetOpacity() const
{
    return this->opacity;
}
