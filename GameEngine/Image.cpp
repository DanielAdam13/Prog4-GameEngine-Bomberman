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
	const glm::vec2 transformPosition{ pOwnerTransform->GetWorldPosition().x, pOwnerTransform->GetWorldPosition().y };

	const glm::vec2 texSize{ m_pTexture->GetSize() };
	const glm::vec2 scaledSize{ texSize * glm::vec2{pOwnerTransform->GetWorldScale().x, pOwnerTransform->GetWorldScale().y}};

	// Has to use Renderer
	Renderer::GetInstance().RenderTexture(*this->GetTexture(), 
		transformPosition.x, transformPosition.y,
		scaledSize.x, scaledSize.y);
}

void Image::SetTexture(Texture2D* texture)
{
	m_pTexture = texture;
}
