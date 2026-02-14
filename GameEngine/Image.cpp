#include "Image.h"

using namespace ge;

void Image::SetTexture(std::shared_ptr<Texture2D> texture)
{
	m_Texture = texture;
}
