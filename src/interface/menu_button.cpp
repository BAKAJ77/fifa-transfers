#include <interface/menu_button.h>
#include <graphics/renderer.h>

MenuButton::MenuButton() :
	currentFontSize(0.0f), baseFontSize(0.0f), sfxPlayed(false)
{}

MenuButton::MenuButton(const glm::vec2& pos, const glm::vec2& baseSize, const glm::vec2& maxSize, const std::string_view& text, const glm::vec3& baseColor, 
	const glm::vec3& highlightColor, const glm::vec3& edgeColor, float opacity, bool noShadow) :
	ButtonBase(pos, baseSize, maxSize, baseColor, highlightColor, edgeColor, opacity, 5.0f * (int)(!noShadow)), text(text), 
	baseFontSize(baseSize.y / 2.5f), currentFontSize(baseSize.y / 2.5f), sfxPlayed(false)
{
	this->sfx = AudioSystem::GetInstance().GetAudio("Switch");

	this->textFont = FontLoader::GetInstance().GetFont("Cascadia Code Bold");
	this->textSize = Renderer::GetInstance().GetTextSize(this->textFont, (uint32_t)this->currentFontSize, text);
}

void MenuButton::Update(const float& deltaTime, float animationSpeed)
{
	this->UpdateButtonAnimation(deltaTime, animationSpeed);

	// Play sound effect when the button is hovered over
	if (this->hovering && !this->sfxPlayed)
	{
		this->sfx->Play();
		this->sfxPlayed = true;
	}
	else if (!this->hovering)
		this->sfxPlayed = false;

	// Animate the size of the text based on if the button is being hovered over
	const float sizeChangeLimit = (this->maxSize.y - this->baseSize.y) / 3.0f;

	if (this->hovering)
		this->currentFontSize = std::min(this->currentFontSize + (sizeChangeLimit * animationSpeed * deltaTime),
			this->baseFontSize + sizeChangeLimit);
	else
		this->currentFontSize = std::max(this->currentFontSize + (-sizeChangeLimit * animationSpeed * deltaTime), this->baseFontSize);

	// Get the current size of the button text
	this->textSize = Renderer::GetInstance().GetTextSize(this->textFont, (uint32_t)this->currentFontSize, text);
}

void MenuButton::Render(float masterOpacity) const
{
	this->RenderButtonSurface(masterOpacity);

	// Render the button text
	constexpr float offset = 7.5f;
	Renderer::GetInstance().RenderText({ this->position.x - (this->textSize.x / 2) + offset, this->position.y + (this->textSize.y / 2) }, 
		glm::vec4(255, 255, 255, (this->opacity * masterOpacity) / 255.0f), this->textFont, (uint32_t)this->currentFontSize, this->text);
}

const std::string& MenuButton::GetText() const
{
	return this->text;
}
