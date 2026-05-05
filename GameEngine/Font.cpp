#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "Font.h"

using namespace ge;

TTF_Font* Font::GetFont() const 
{
	return m_Font;
}

Font::Font(const std::string& fullPath, float size) 
	:m_Font{ nullptr },
	m_FontPath{ fullPath }
{
	m_Font = TTF_OpenFont(m_FontPath.c_str(), size);
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

void Font::SetFontSize(float newSize)
{
	if (m_Font)
	{
		TTF_CloseFont(m_Font);
	}

	m_Font = TTF_OpenFont(m_FontPath.c_str(), newSize);
	if (m_Font == nullptr)
	{
		throw std::runtime_error(std::string("Failed to load font: ") + SDL_GetError());
	}
}

void ge::Font::SetBold(bool bold)
{
	if (!m_Font) 
		return;

	TTF_FontStyleFlags style{ TTF_GetFontStyle(m_Font) };

	if (bold)
		style |= TTF_STYLE_BOLD;
	else
		style &= ~TTF_STYLE_BOLD;

	TTF_SetFontStyle(m_Font, style);
}
