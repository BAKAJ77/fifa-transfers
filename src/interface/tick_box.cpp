#include <interface/tick_box.h>
#include <graphics/renderer.h>

TickBox::TickBox() :
    isTicked(false)
{}

TickBox::TickBox(const glm::vec2& pos, const glm::vec2& buttonSize, const std::string_view& label, float opacity, float boxOffset, bool defaultValue) :
    ButtonBase(pos, buttonSize, buttonSize, glm::vec3(70), glm::vec3(100), glm::vec3(130), opacity), labelText(label), isTicked(defaultValue)
{
    this->font = FontLoader::GetInstance().GetFont("Bahnschrift Bold");
    const glm::vec2 labelTextSize = Renderer::GetInstance().GetTextSize(this->font, (uint32_t)buttonSize.y, label);
    this->labelTextPosition = { pos.x, pos.y + (labelTextSize.y / 2) };

    // Adjust the position of the tick box so it's adjacent to the label text
    this->position.x = pos.x + labelTextSize.x + (this->baseSize.x / 2) + boxOffset;
}

void TickBox::Reset()
{
    this->isTicked = false;
}

void TickBox::Update(const float& deltaTime, float animationSpeed)
{
    if (this->opacity > 0)
    {
        this->UpdateButtonAnimation(deltaTime, animationSpeed);

        if (this->clicked)
            this->isTicked = !this->isTicked; // Invert the isTicked boolean state
    }
}

void TickBox::Render(float masterOpacity) const
{
    // Render the label text
    Renderer::GetInstance().RenderShadowedText(this->labelTextPosition, { glm::vec3(255), (this->opacity * masterOpacity) / 255 }, this->font, 
        (uint32_t)this->baseSize.y, this->labelText, 5);
    
    // Render the tick box surface
    this->RenderButtonSurface(masterOpacity);

    if (this->isTicked)
        Renderer::GetInstance().RenderSquare(this->position, this->baseSize / 2.5f, { 255, 0, 0, (this->opacity * masterOpacity) / 255.0f });
}

bool TickBox::isCurrentlyTicked() const
{
    return this->isTicked;
}