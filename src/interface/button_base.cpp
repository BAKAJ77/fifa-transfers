#include <interface/button_base.h>
#include <graphics/renderer.h>
#include <core/input_system.h>

#include <algorithm>

namespace Events
{
    static bool released = false;
}

ButtonBase::ButtonBase() :
    shadowDistance(0.0f), opacity(0.0f), hovering(false), clicked(false)
{}

ButtonBase::ButtonBase(const glm::vec2& pos, const glm::vec2& baseSize, const glm::vec2& maxSize, const glm::vec3& baseColor,
    const glm::vec3& highlightColor, const glm::vec3& edgeColor, float opacity, float shadowDistance) :
    position(pos), baseSize(baseSize), currentSize(baseSize), maxSize(maxSize), currentColor(baseColor), baseColor(baseColor),
    highlightColor(highlightColor), edgeColor(edgeColor), shadowDistance(shadowDistance), opacity(opacity), hovering(false), clicked(false)
{}

void ButtonBase::SetPosition(const glm::vec2& pos)
{
    this->position = pos;
}

void ButtonBase::SetBaseSize(const glm::vec2& size)
{
    this->baseSize = size;
}

void ButtonBase::SetMaxSize(const glm::vec2& size)
{
    this->maxSize = size;
}

void ButtonBase::SetBaseColor(const glm::vec4& color)
{
    this->baseColor = color;
}

void ButtonBase::SetEdgeColor(const glm::vec4& color)
{
    this->edgeColor = color;
}

void ButtonBase::SetHighlightColor(const glm::vec4& color)
{
    this->highlightColor = color;
}

void ButtonBase::SetShadowDistance(float distance)
{
    this->shadowDistance = distance;
}

void ButtonBase::SetOpacity(float opacity)
{
    this->opacity = opacity;
}

void ButtonBase::Update(const float& deltaTime, float animationSpeed)
{
    if (this->opacity > 0)
        this->UpdateButtonAnimation(deltaTime, animationSpeed);
}

void ButtonBase::Render(float masterOpacity) const
{
    this->RenderButtonSurface(masterOpacity);
}

void ButtonBase::CheckButtonClicked()
{
    if (InputSystem::GetInstance().WasMouseButtonPressed(MouseCode::MOUSE_BUTTON_LEFT) && Events::released)
    {
        this->clicked = true;
        Events::released = false;
    }
    else
        this->clicked = false;
}

void ButtonBase::CheckMouseClickReleased()
{
    if (!InputSystem::GetInstance().WasMouseButtonPressed(MouseCode::MOUSE_BUTTON_LEFT) && !Events::released)
        Events::released = true;
}

glm::vec2 ButtonBase::Interpolate(const glm::vec2& vec, const glm::vec2& baseVec, const glm::vec2& targetVec, float animationSpeed, 
    const float& deltaTime) const
{
    glm::vec3 vector = this->Interpolate(glm::vec3(vec, 0), glm::vec3(baseVec, 0), glm::vec3(targetVec, 0), animationSpeed, deltaTime);
    return { vector.x, vector.y };
}

glm::vec3 ButtonBase::Interpolate(const glm::vec3& vec, const glm::vec3& baseVec, const glm::vec3& targetVec, float animationSpeed,
    const float& deltaTime) const
{
    glm::vec3 vector = vec;
    const glm::vec3 diff = targetVec - baseVec;

    diff.x > 0.0f ? vector.x = std::min(vector.x + (diff.x * animationSpeed * deltaTime), targetVec.x) :
        vector.x = std::max(vector.x + (diff.x * animationSpeed * deltaTime), targetVec.x);
    diff.y > 0.0f ? vector.y = std::min(vector.y + (diff.y * animationSpeed * deltaTime), targetVec.y) :
        vector.y = std::max(vector.y + (diff.y * animationSpeed * deltaTime), targetVec.y);
    diff.z > 0.0f ? vector.z = std::min(vector.z + (diff.z * animationSpeed * deltaTime), targetVec.z) :
        vector.z = std::max(vector.z + (diff.z * animationSpeed * deltaTime), targetVec.z);

    return vector;
}

void ButtonBase::UpdateButtonAnimation(const float& deltaTime, float animationSpeed)
{
    constexpr float maxBrightnessFactor = 1.5f;
    constexpr float brightnessDifference = (maxBrightnessFactor - 1.0f);

    // Get the cursor position
    const glm::vec2 cursorPosition = InputSystem::GetInstance().GetCursorPosition(&Renderer::GetInstance().GetViewport());

    // Check if the cursor is hovering over the button
    if (cursorPosition.x >= this->position.x - (this->currentSize.x / 2.0f) &&
        cursorPosition.x <= this->position.x + (this->currentSize.x / 2.0f) &&
        cursorPosition.y >= this->position.y - (this->currentSize.y / 2.0f) &&
        cursorPosition.y <= this->position.y + (this->currentSize.y / 2.0f))
    {
        this->hovering = true;

        // Animate the button color's gradual change to the highlight color
        this->currentColor = this->Interpolate(this->currentColor, this->baseColor, this->highlightColor, animationSpeed, deltaTime);

        // Animate the button's gradual change to the max size
        this->currentSize = this->Interpolate(this->currentSize, this->baseSize, this->maxSize, animationSpeed, deltaTime);

        // Check if the button has been clicked
        this->CheckButtonClicked();
    }
    else
    {
        this->hovering = false;
        this->clicked = false;

        // Animate the button color's gradual change to the base color
        this->currentColor = this->Interpolate(this->currentColor, this->highlightColor, this->baseColor, animationSpeed, deltaTime);

        // Animate the button's gradual change to the base size
        this->currentSize = this->Interpolate(this->currentSize, this->maxSize, this->baseSize, animationSpeed, deltaTime);
    }

    // The user must release the left mouse button before being able to click the button again.
    // This prevents the click boolean from being constantly set as TRUE if the user holds the left mouse button on an interface button.
    this->CheckMouseClickReleased();
}

void ButtonBase::RenderButtonSurface(float masterOpacity) const
{
    // Render the shadow of the button
    if (this->shadowDistance > 0.0f)
        Renderer::GetInstance().RenderSquare(this->position + (this->shadowDistance * 2.0f), this->currentSize,
            glm::vec4(glm::vec3(0.0f), ((this->opacity * masterOpacity) / 255.0f) * 0.5f));

    // Render the edge of the button
    Renderer::GetInstance().RenderSquare(this->position, this->currentSize, glm::vec4(this->edgeColor, 
        ((this->opacity * masterOpacity) / 255.0f)));

    // Render the body of the button
    Renderer::GetInstance().RenderSquare(this->position, this->currentSize - 2.0f, glm::vec4(this->currentColor, 
        ((this->opacity * masterOpacity) / 255.0f)));
}

bool ButtonBase::WasClicked() const
{
    return this->clicked;
}

bool ButtonBase::IsCursorHovering() const
{
    return this->hovering;
}

const glm::vec2& ButtonBase::GetPosition() const
{
    return this->position;
}

const glm::vec2& ButtonBase::GetCurrentSize() const
{
    return this->currentSize;
}

const glm::vec2& ButtonBase::GetBaseSize() const
{
    return this->baseSize;
}

const glm::vec2& ButtonBase::GetMaxSize() const
{
    return this->maxSize;
}

const glm::vec3& ButtonBase::GetBaseColor() const
{
    return this->baseColor;
}

const glm::vec3& ButtonBase::GetEdgeColor() const
{
    return this->edgeColor;
}

const glm::vec3& ButtonBase::GetHighlightColor() const
{
    return this->highlightColor;
}

const float& ButtonBase::GetShadowDistance() const
{
    return this->shadowDistance;
}

const float& ButtonBase::GetOpacity() const
{
    return this->opacity;
}
