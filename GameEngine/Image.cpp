#include "Image.h"
#include "Renderer.h"

using namespace ge;

Image::~Image()
{
	m_pTexture = nullptr;
}

void Image::RenderComponent(const glm::vec3 transformPos) const
{
	Renderer::GetInstance().RenderTexture(*this->GetTexture(), transformPos.x, transformPos.y);
}

void Image::SetTexture(Texture2D* texture)
{
	m_pTexture = texture;
}
