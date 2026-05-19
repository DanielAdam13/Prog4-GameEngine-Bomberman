#pragma once
#include "Singleton.h"
#include "ObservableSubject.h"

#include <SDL3/SDL.h>
#include <utility>

namespace ge
{
	class Texture2D;

	/**
	 * RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
	public:
		void Init(SDL_Window* window, const int windowX, const int windowY);
		void Render() const;
		void Destroy();

		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;
		void RenderRectOutline(float x, float y, float w, float h, const SDL_Color& color) const;
		void RenderTextureRegion(const Texture2D& texture, const SDL_FRect& srcRect,
			float x, float y, float w, float h) const;

		SDL_Renderer* GetSDLRenderer() const { return m_Renderer; };

		const SDL_Color& GetBackgroundColor() const { return m_ClearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_ClearColor = color; }

		std::pair<int, int> GetWindowSize() const;

		void SetWindowSize(std::pair<int, int>);

		Subject& GetOnScreenResizeEvent() noexcept;
		std::pair<float, float> GetWindowDesignSize() const noexcept;

	private:
		SDL_Renderer* m_Renderer{};
		SDL_Window* m_Window{}; // Doesn't own
		SDL_Color m_ClearColor{};

		std::pair<int, int> m_CurrentWindowSize;
		std::pair<float, float> m_RenderScale;
		std::pair<float, float> m_ConstantDesignSize;
	
		Subject m_OnScreenResizeEvent;
	};
}

