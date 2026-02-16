#include "Image.h"
#include "Renderer.h"

using namespace ge;

void Image::RenderComponent(const glm::vec3& transformPos) const
{
	// Has to use Renderer Singleton
	Renderer::GetInstance().RenderTexture(*this->GetTexture(), transformPos.x, transformPos.y);
}

void Image::SetTexture(Texture2D* texture)
{
	m_pTexture = texture;
}
