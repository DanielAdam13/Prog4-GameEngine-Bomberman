#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "Font.h"

using namespace ge;

TTF_Font* Font::GetFont() const 
{
	return m_Font;
}

Font::Font(const std::string& fullPath, float size) 
	:m_Font{ nullptr }
{
	m_Font = TTF_OpenFont(fullPath.c_str(), size);
	if (m_Font == nullptr) 
	{
		throw std::runtime_error(std::string("Failed to load font: ") + SDL_GetError());
	}
}

Font::~Font()
{
	if (m_Font)
	{
		TTF_CloseFont(m_Font);
		m_Font = nullptr;
	}
}
