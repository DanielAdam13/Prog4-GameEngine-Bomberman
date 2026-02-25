#include "Image.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Transform.h"

using namespace ge;

Image::Image(GameObject* pOwnerPtr)
	:Component::Component(pOwnerPtr)
{
}

void Image::RenderComponent() const
{
	auto pOwnerTransform{ GetOwner()->GetComponent<Transform>() };
	const glm::vec2 worldTransPos{ pOwnerTransform->GetWorldPosition().x, pOwnerTransform->GetWorldPosition().y };

	// Has to use Renderer
	Renderer::GetInstance().RenderTexture(*this->GetTexture(), worldTransPos.x, worldTransPos.y);
}

void Image::SetTexture(Texture2D* texture)
{
	m_pTexture = texture;
}
