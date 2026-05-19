#include "SpriteSheet.h"
#include "Texture2D.h"

#include <cassert>

ge::SpriteSheet::SpriteSheet(Texture2D* spriteTexture, int columns, int rows)
	:m_TextureSheet{ spriteTexture },
	m_Columns{ columns },
	m_Rows{ rows }
{
	if (!m_TextureSheet || columns == 0 || rows == 0)
		assert("Cannot create invalid sprite sheet!");

	m_FrameWidth = m_TextureSheet->GetSize().x / m_Columns;
	m_FrameHeight = m_TextureSheet->GetSize().y / m_Rows;
}
