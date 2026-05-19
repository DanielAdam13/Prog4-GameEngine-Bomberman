#pragma once

#include <SDL3/SDL_rect.h>

namespace ge
{
	class Texture2D;

	// Wrapper of Texture2D. 
	// Data struct describing HOW to slice a texture
	class SpriteSheet final
	{
	public:
		SpriteSheet(Texture2D* spriteTexture, int columns, int rows);
		~SpriteSheet() = default;
		SpriteSheet(const SpriteSheet&) = delete;
		SpriteSheet(SpriteSheet&&) = delete;
		SpriteSheet& operator= (const SpriteSheet&) = delete;
		SpriteSheet& operator= (SpriteSheet&&) = delete;

		Texture2D* GetTexture() const noexcept { return m_TextureSheet; }
		int GetColumns() const noexcept { return m_Columns; }
		int GetRows() const noexcept { return m_Rows; }
		int GetFrameCount() const noexcept { return m_Columns * m_Rows; }
		float GetFrameWidth() const noexcept { return m_FrameWidth; }
		float GetFrameHeight() const noexcept { return m_FrameHeight; }

		SDL_FRect GetFrameRect(int frameIndex) const noexcept;

	private:
		Texture2D* m_TextureSheet; // Not owning, reference
		int m_Columns;
		int m_Rows;

		float m_FrameWidth;
		float m_FrameHeight;

	};
}