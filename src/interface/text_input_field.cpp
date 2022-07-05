#include <interface/text_input_field.h>
#include <core/input_system.h>
#include <graphics/renderer.h>
#include <util/timestamp.h>

namespace Focus
{
    static TextInputField* focusedTextField = nullptr;
}

TextInputField::TextInputField() :
    opacity(0.0f), shadowDistance(0.0f), fontSize(0.0f), inputFlags(Restrictions::NONE)
{}

TextInputField::TextInputField(const glm::vec2& pos, const glm::vec2& size, Restrictions flags, float opacity, float shadowDistance) :
    position(pos), size(size), opacity(opacity), shadowDistance(shadowDistance), fontSize(size.y / 2.0f), inputFlags(flags)
{
    this->textFont = FontLoader::GetInstance().GetFont("Bahnschrift Bold");
}

TextInputField::~TextInputField() {}

void TextInputField::SetFocus(bool focused)
{
    Focus::focusedTextField = this;
}

void TextInputField::SetPosition(const glm::vec2& pos)
{
    this->position = pos;
}

void TextInputField::SetSize(const glm::vec2& size)
{
    this->size = size;
}

void TextInputField::SetOpacity(float opacity)
{
    this->opacity = opacity;
}

void TextInputField::SetShadowDistance(float distance)
{
    this->shadowDistance = distance;
}

void TextInputField::Clear()
{
    this->inputtedText.clear();
}

void TextInputField::Update(const float& deltaTime)
{
    if (this->IsFocused())
    {
        // Poll for any character that has been inputted
        const uint32_t character = InputSystem::GetInstance().GetInputtedCharacter();

        // Accept only spaces, numerical and alphabetic characters based on the input restriction flags given
        // Also only accept characters if the text box field is not full
        if (this->textSize.x < this->size.x - 50)
        {
            if ((character == 32 && (this->inputFlags & Restrictions::NO_SPACES) != Restrictions::NO_SPACES) ||
                ((character >= 48 && character <= 57) && (this->inputFlags & Restrictions::NO_NUMERIC) != Restrictions::NO_NUMERIC) ||
                (((character >= 65 && character <= 90) || (character >= 97 && character <= 122)) &&
                    (this->inputFlags & Restrictions::NO_ALPHABETIC) != Restrictions::NO_ALPHABETIC))
            {
                this->inputtedText.push_back((char)character);
            }
        }

        // Pop the last character if the user presses BACKSPACE
        static float previousTime = Util::GetSecondsSinceEpoch();
        if (Util::GetSecondsSinceEpoch() - previousTime >= 0.2f)
        {
            if (InputSystem::GetInstance().WasKeyPressed(KeyCode::KEY_BACKSPACE) && !this->inputtedText.empty())
            {
                this->inputtedText.pop_back();
                previousTime = Util::GetSecondsSinceEpoch();
            }
        }

        // Get the size of the text to be rendered
        this->textSize = Renderer::GetInstance().GetTextSize(this->textFont, (uint32_t)this->fontSize, this->inputtedText);
    }

    // Set this text field as focused if it has been clicked
    if (this->WasClicked())
    {
        this->SetFocus(true);
        InputSystem::GetInstance().GetInputtedCharacter(); // Clear any pending inputted characters
    }
}

void TextInputField::Render(float masterOpacity) const
{
    // Render the shadow of the text box
    Renderer::GetInstance().RenderSquare(this->position + (this->shadowDistance * 2.0f), this->size,
        glm::vec4(glm::vec3(0.0f), ((this->opacity * masterOpacity) / 255.0f) * 0.5f));

    // Render the outline of the text box
    this->IsFocused() ?
        Renderer::GetInstance().RenderSquare(this->position, this->size, { 255, 0, 0, (this->opacity * masterOpacity) / 255.0f }) :
        Renderer::GetInstance().RenderSquare(this->position, this->size, { 150, 150, 150, (this->opacity * masterOpacity) / 255.0f });

    // Render the inner body of the text box
    Renderer::GetInstance().RenderSquare(this->position, this->size - 10.0f, { 255, 255, 255, (this->opacity * masterOpacity) / 255.0f });

    // Render the inputted text string in the text box
    Renderer::GetInstance().RenderText({ this->position.x - (this->size.x / 2) + (30 * (this->size.x / 700.0f)),
        this->position.y + (17.5f * (this->size.y / 100.0f)) },
        { glm::vec3(0.0f), (this->opacity * masterOpacity) / 255.0f }, this->textFont, (uint32_t)fontSize, this->inputtedText);
}

bool TextInputField::WasClicked() const
{
    static bool released = true;

    // Get the cursor position
    const glm::vec2 cursorPosition = InputSystem::GetInstance().GetCursorPosition(&Renderer::GetInstance().GetViewport());

    // Check if the cursor is hovering over the button
    if (cursorPosition.x >= this->position.x - (this->size.x / 2.0f) && cursorPosition.x <= this->position.x + (this->size.x / 2.0f) &&
        cursorPosition.y >= this->position.y - (this->size.y / 2.0f) && cursorPosition.y <= this->position.y + (this->size.y / 2.0f))
    {
        // Check if the button has been clicked
        if (InputSystem::GetInstance().WasMouseButtonPressed(MouseCode::MOUSE_BUTTON_LEFT) && released)
        {
            released = false;
            return true;
        }
    }

    // The user must release the left mouse button before being able to click the text field again.
    if (!InputSystem::GetInstance().WasMouseButtonPressed(MouseCode::MOUSE_BUTTON_LEFT) && !released)
        released = true;

    return false;
}

bool TextInputField::IsFocused() const
{
    return Focus::focusedTextField == this;
}

const std::string& TextInputField::GetInputtedText() const
{
    return this->inputtedText;
}

const glm::vec2& TextInputField::GetPosition() const
{
    return this->position;
}

const glm::vec2& TextInputField::GetSize() const
{
    return this->size;
}

const float& TextInputField::GetOpacity() const
{
    return this->opacity;
}

const float& TextInputField::GetShadowDistance() const
{
    return this->shadowDistance;
}

TextInputField::Restrictions operator&(const TextInputField::Restrictions& lhs, const TextInputField::Restrictions& rhs)
{
    return (TextInputField::Restrictions)((uint8_t)lhs & (uint8_t)rhs);
}

TextInputField::Restrictions operator|(const TextInputField::Restrictions& lhs, const TextInputField::Restrictions& rhs)
{
    return (TextInputField::Restrictions)((uint8_t)lhs | (uint8_t)rhs);
}
