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
	if (!m_pTexture)
		return;

	auto pOwnerTransform{ GetOwner()->GetComponent<Transform>() };
	const glm::vec2 worldPos{ pOwnerTransform->GetWorldPosition().x, 
		pOwnerTransform->GetWorldPosition().y };
	const glm::vec2 worldScale{ pOwnerTransform->GetWorldScale().x, pOwnerTransform->GetWorldScale().y };

	const glm::vec2 texSize{ m_pTexture->GetSize() };
	const glm::vec2 scaledSize{ texSize * worldScale };

	const float topLeftX{ worldPos.x - m_Anchor.x * scaledSize.x };
	const float topLeftY{ worldPos.y - m_Anchor.y * scaledSize.y };

	// Has to use Renderer
	Renderer::GetInstance().RenderTexture(*this->GetTexture(), 
		topLeftX, topLeftY,
		scaledSize.x, scaledSize.y);
}

void Image::SetTexture(Texture2D* texture)
{
	m_pTexture = texture;
}

void ge::Image::SetAnchor(const glm::vec2& anchor) noexcept
{
	m_Anchor = anchor;
}

const glm::vec2 ge::Image::GetAnchor() const noexcept
{
	return m_Anchor;
}
