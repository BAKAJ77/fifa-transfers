#include <graphics/orthogonal_camera.h>
#include <glm/gtc/matrix_transform.hpp>

OrthogonalCamera::OrthogonalCamera(const glm::vec2& pos, const glm::vec2& size) :
	position(pos), size(size)
{
	this->projectionMatrix = glm::ortho(0.0f, size.x, size.y, 0.0f);
}

void OrthogonalCamera::SetPosition(const glm::vec2& pos)
{
	this->position = pos;
}

void OrthogonalCamera::SetSize(const glm::vec2& size)
{
	this->size = size;
}

glm::mat4 OrthogonalCamera::GetMatrix() const
{
	return this->projectionMatrix * this->GetViewMatrix();
}

glm::mat4 OrthogonalCamera::GetViewMatrix() const
{
	return glm::lookAt(glm::vec3(this->position, 0.0f), glm::vec3(this->position, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

float OrthogonalCamera::GetAspectRatio() const
{
	return this->size.x / this->size.y;
}

const glm::mat4& OrthogonalCamera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

const glm::vec2& OrthogonalCamera::GetPosition() const
{
	return this->position;
}

const glm::vec2& OrthogonalCamera::GetSize() const
{
	return this->size;
}
