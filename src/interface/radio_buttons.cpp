#include <interface/radio_buttons.h>
#include <util/logging_system.h>
#include <graphics/renderer.h>

RadioButtonGroup::RadioButtonGroup() :
    currentSelected(nullptr), currentOffset(0.0f), opacity(0.0f)
{}

RadioButtonGroup::RadioButtonGroup(const glm::vec2& pos, const glm::vec2& buttonSize, float opacity) :
    position(pos), buttonSize(buttonSize), currentSelected(nullptr), currentOffset(0.0f), opacity(opacity)
{
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");
}

void RadioButtonGroup::SetOpacity(float opacity)
{
    this->opacity = opacity;
}

void RadioButtonGroup::Add(const std::string_view& id)
{
    // Don't add the radio button to the group if another with the ID given already exists
    for (const RadioButton& button : this->radioButtons)
    {
        if (button.id == id)
        {
            LogSystem::GetInstance().OutputLog("There's already a radio button with the ID: " + std::string(id), Severity::WARNING);
            return;
        }
    }

    // Get the size of the ID text
    const glm::vec2 idTextSize = Renderer::GetInstance().GetTextSize(this->font, (uint32_t)(this->buttonSize.y), id);

    // Push the radio button into the vector
    this->radioButtons.push_back({ id.data(), idTextSize, ButtonBase({ this->position.x + this->currentOffset, this->position.y }, this->buttonSize, 
        this->buttonSize, glm::vec3(70), glm::vec3(100), glm::vec3(130), this->opacity, 2.5f) });

    // Update the current offset
    this->currentOffset += idTextSize.x + (this->buttonSize.x / 2) + 70;
}

void RadioButtonGroup::Update(const float& deltaTime)
{
    for (RadioButton& button : this->radioButtons)
    {
        // Update the radio button
        button.button.Update(deltaTime, 8.0f);

        // If the radio button was clicked, set it as current selected
        if (button.button.WasClicked())
            this->currentSelected = &button;
    }
}

void RadioButtonGroup::Render(float masterOpacity) const
{
    for (const RadioButton& button : this->radioButtons)
    {
        // Render the radio button
        button.button.Render(masterOpacity);

        // If the radio button is currently selected, draw dot in centre of it
        if (this->currentSelected == &button)
            Renderer::GetInstance().RenderSquare(button.button.GetPosition(), button.button.GetCurrentSize() / 2.5f,
                { 255, 0, 0, (this->opacity * masterOpacity) / 255.0f });
        
        // Render the id text next to it
        Renderer::GetInstance().RenderShadowedText({ button.button.GetPosition().x + (this->buttonSize.x / 2) + 25, 
            button.button.GetPosition().y + (button.textSize.y / 2)}, { glm::vec3(255), (this->opacity * masterOpacity) / 255.0f }, this->font, 
            (uint32_t)(this->buttonSize.y), button.id, 5);
    }
}

std::string_view RadioButtonGroup::GetSelected() const
{
    if (this->currentSelected)
        return this->currentSelected->id;

    return std::string_view();
}

const float& RadioButtonGroup::GetOpacity() const
{
    return this->opacity;
}
