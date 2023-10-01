#include <interface/drop_down.h>
#include <core/input_system.h>
#include <graphics/renderer.h>
#include <util/logging_system.h>

DropDown::DropDown() :
    opacity(0.0f), doDropDown(false), fontSize(0), clicked(false), released(false), selectionsOffset(0), maxSelectionsVisible(0), textOffset(0.0f)
{}

DropDown::DropDown(const glm::vec2& pos, const glm::vec2& size, float opacity, float fontSize, float textOffset) :
    position(pos), size(size), opacity(opacity), doDropDown(false), clicked(false), released(false), fontSize(fontSize <= 0.0f ? size.y / 2.5f : fontSize), 
    textOffset(textOffset), selectionsOffset(0)
{
    // Load the font to be used
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");

    // Calculate the max amount of selection options that can be displayed at a time when dropped down
    this->maxSelectionsVisible = (int)((1080.0f - pos.y - (size.y / 2)) / size.y);

    // Set empty selection as default
    this->currentSelected = { std::string(), 
        { pos, size, size, glm::vec3(60), glm::vec3(90), glm::vec3(120), this->opacity, 2.5f }, glm::vec2(0), -1 };
}

DropDown::DropDown(DropDown&& temp) noexcept :
    font(std::move(temp.font)), position(std::move(temp.position)), size(std::move(temp.size)), fontSize(std::move(temp.fontSize)), 
    opacity(std::move(temp.opacity)), selections(std::move(temp.selections)), currentSelected(std::move(temp.currentSelected)), clicked(false), 
    released(false), doDropDown(false), selectionsOffset(0), maxSelectionsVisible(std::move(temp.maxSelectionsVisible)), 
    textOffset(std::move(temp.textOffset))
{}

DropDown& DropDown::operator=(DropDown&& temp) noexcept
{
    this->font = std::move(temp.font);
    this->position = std::move(temp.position);
    this->size = std::move(temp.size);
    this->fontSize = std::move(temp.fontSize);
    this->textOffset = std::move(temp.textOffset);
    this->opacity = std::move(temp.opacity);
    
    this->maxSelectionsVisible = std::move(temp.maxSelectionsVisible);
    this->selections = std::move(temp.selections);
    this->currentSelected = std::move(temp.currentSelected);

    return *this;
}

void DropDown::SetOpacity(float opacity)
{
    this->opacity = opacity;
    this->currentSelected.button.SetOpacity(opacity);

    for (Element& element : this->selections)
        element.button.SetOpacity(opacity);
}

void DropDown::AddSelection(const std::string_view& id, int value)
{
    this->selections.push_back({ id.data(), { this->position, this->size, this->size, glm::vec3(85), glm::vec3(115), glm::vec3(85), this->opacity, 2.5f }, 
        Renderer::GetInstance().GetTextSize(this->font, (uint32_t)this->fontSize, id), value });
}

void DropDown::Clear()
{
    this->selections.clear();
    this->selectionsOffset = 0;
    this->doDropDown = false;

    // Set empty selection as default
    this->currentSelected = { std::string(),
        { this->position, size, size, glm::vec3(60), glm::vec3(90), glm::vec3(120), this->opacity, 2.5f }, glm::vec2(0), -1 };
}

void DropDown::Reset()
{
    this->currentSelected = { std::string(),
        { this->position, this->size, this->size, glm::vec3(60), glm::vec3(90), glm::vec3(120), this->opacity, 2.5f }, glm::vec2(0), -1 };
}

void DropDown::Update(const float& deltaTime)
{
    if (this->opacity > 0)
    {
        // Update the selections offset via the user scrolling
        if (this->doDropDown)
        {
            const float scrollOffsetY = InputSystem::GetInstance().GetScrollOffset().y;

            if (scrollOffsetY > 0.0f)
                this->selectionsOffset = std::max(this->selectionsOffset - 0.075f, 0.0f);
            else if (scrollOffsetY < 0.0f)
                this->selectionsOffset = std::min(this->selectionsOffset + 0.075f, (float)std::max((int)this->selections.size() - this->maxSelectionsVisible, 0));
        }

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
            for (int i = (int)this->selectionsOffset; i < std::min((int)this->selectionsOffset + this->maxSelectionsVisible, (int)this->selections.size()); i++)
            {
                this->selections[i].button.SetPosition({ this->position.x, this->position.y + ((float)((i - (int)this->selectionsOffset) + 1) * this->size.y) });
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
}

void DropDown::Render(float masterOpacity) const
{
    // Render the current selection button
    this->currentSelected.button.Render(masterOpacity);

    // Render the current selection button text
    Renderer::GetInstance().RenderText({ this->position.x - (this->currentSelected.textSize.x / 2) + this->textOffset,
        this->position.y + (this->currentSelected.textSize.y / 2) }, glm::vec4(255, 255, 255, (this->opacity * masterOpacity) / 255.0f), this->font,
        (uint32_t)this->fontSize, this->currentSelected.id);

    if (this->doDropDown)
    {
        for (int i = (int)this->selectionsOffset; i < std::min((int)this->selectionsOffset + this->maxSelectionsVisible, (int)this->selections.size()); i++)
        {
            // Render the selection element button body
            this->selections[i].button.Render(masterOpacity);

            // Render the selection element button text
            Renderer::GetInstance().RenderText({ this->selections[i].button.GetPosition().x - (this->selections[i].textSize.x / 2) + this->textOffset,
                this->selections[i].button.GetPosition().y + (this->selections[i].textSize.y / 2) }, 
                glm::vec4(255, 255, 255, (this->opacity * masterOpacity) / 255.0f), this->font, (uint32_t)this->fontSize, this->selections[i].id);
        }

        // If drop down is scrollable, render small arrow triangles to indicate so
        if (this->selections.size() > this->maxSelectionsVisible)
        {
            if ((int)this->selectionsOffset > 0)
                Renderer::GetInstance().RenderTriangle({ this->position.x + (this->size.x / 2) - 20, this->position.y + (this->size.y / 2) + 20 },
                    glm::vec2(20, 25), { 50, 50, 50, (this->opacity * masterOpacity) / 255 }, 180);

            if (((int)this->selectionsOffset + this->maxSelectionsVisible) < this->selections.size())
                Renderer::GetInstance().RenderTriangle({ this->position.x + (this->size.x / 2) - 20, 
                    this->position.y + ((this->maxSelectionsVisible * this->size.y) + (this->size.y / 2)) - 20}, glm::vec2(20, 25), 
                    { 50, 50, 50, (this->opacity * masterOpacity) / 255 });
        }
    }
}

int DropDown::GetCurrentSelected() const
{
    return this->currentSelected.value;
}

bool DropDown::IsDroppedDown() const
{
    return this->doDropDown;
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
