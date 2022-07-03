#ifndef BUTTON_BASE_H
#define BUTTON_BASE_H

#include <glm/glm.hpp>
#include <string>

class ButtonBase
{
protected:
	glm::vec2 position, currentSize, baseSize, maxSize;
	glm::vec3 currentColor, baseColor, highlightColor, edgeColor;

	float shadowDistance, opacity;
	bool clicked, hovering;
private:
	// Checks if the button has been clicked.
	void CheckButtonClicked();

	// Check if the left mouse button has been released after clicking the button.
	void CheckMouseClickReleased();
protected:
	// Updates the animation of the button when it is being hovered over.
	void UpdateButtonAnimation(const float& deltaTime, float animationSpeed);

	// Renders the button's body, edge/border and shadow.
	void RenderButtonSurface(float masterOpacity) const;

	// Returns a vector interpolated from the base vector, target vector and animation speed.
	glm::vec2 Interpolate(const glm::vec2& vec, const glm::vec2& baseVec, const glm::vec2& targetVec, float animationSpeed,
		const float& deltaTime) const;

	// Returns a vector interpolated from the base vector, target vector and animation speed.
	glm::vec3 Interpolate(const glm::vec3& vec, const glm::vec3& baseVec, const glm::vec3& targetVec, float animationSpeed,
		const float& deltaTime) const;
public:
	ButtonBase();
	ButtonBase(const glm::vec2& pos, const glm::vec2& baseSize, const glm::vec2& maxSize, const glm::vec3& baseColor, 
		const glm::vec3& highlightColor, const glm::vec3& edgeColor, float opacity = 255.0f, float shadowDistance = 5.0f);
	~ButtonBase() = default;

	// Sets the position of the button.
	void SetPosition(const glm::vec2& pos);

	// Sets the base size of the button.
	void SetBaseSize(const glm::vec2& size);

	// Sets the max size of the button.
	void SetMaxSize(const glm::vec2& size);

	// Sets the base color of the button.
	void SetBaseColor(const glm::vec4& color);

	// Sets the color of the button when it is hovered over.
	void SetHighlightColor(const glm::vec4& color);

	// Sets the color of the edge/border of the button.
	void SetEdgeColor(const glm::vec4& color);
	
	// Sets the distance of the shadow from the button.
	void SetShadowDistance(float distance);

	// Sets the opacity of the button.
	void SetOpacity(float opacity);

	// Overridable function which is used for updating the button.
	// Remember to call UpdateButtonAnimation() inside this function if you are overriding it.
	virtual void Update(const float& deltaTime, float animationSpeed);

	// Overridable function which is used for rendering the button.
	// Remember to call RenderButtonSurface() inside this function if you are overriding it.
	virtual void Render(float masterOpacity) const;

	// Returns TRUE if the button was clicked.
	bool WasClicked() const;

	// Returns TRUE if the cursor is hovering over the button.
	bool IsCursorHovering() const;

	// Returns the position of the button.
	const glm::vec2& GetPosition() const;

	// Returns the current size of the button.
	const glm::vec2& GetCurrentSize() const;

	// Returns the base size of the button.
	const glm::vec2& GetBaseSize() const;

	// Returns the max size of the button.
	const glm::vec2& GetMaxSize() const;

	// Returns the base color of the button.
	const glm::vec3& GetBaseColor() const;

	// Returns the color of the button when it is hovered over.
	const glm::vec3& GetHighlightColor() const;

	// Returns the color of the button's edge/border.
	const glm::vec3& GetEdgeColor() const;

	// Returns the distance of the shadow from the button.
	const float& GetShadowDistance() const;

	// Returns the opacity of the button.
	const float& GetOpacity() const;
};

#endif
