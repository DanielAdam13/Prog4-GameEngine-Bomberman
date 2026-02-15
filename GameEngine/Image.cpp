#include "Image.h"

using namespace ge;

Image::~Image()
{
	m_Texture = nullptr;
}

void Image::SetTexture(Texture2D* texture)
{
	m_Texture = texture;
}
