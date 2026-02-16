#pragma once
#include "Component.h"
#include <string>
#include <SDL3/SDL_pixels.h>
#include <memory>

namespace ge
{
	class Font;
	class Texture2D;

	class TextComponent final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Text Component Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 2 };

		TextComponent(const std::string& text, Font* font, const SDL_Color& color = { 255, 255, 255, 255 });
		virtual ~TextComponent() override = default;
		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		// Needs to override both
		virtual void UpdateComponent() override;
		virtual void RenderComponent(const glm::vec3 transformPos) const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

	private:
		std::string m_Text;
		Font* m_pTextFont; // Doesn't own
		SDL_Color m_TextColor;

		// A Text Component OWNS the generated texture since it is unique for the object => ResourceManager doesn't own it
		std::unique_ptr<Texture2D> m_TextTexture;

		bool m_ShouldUpdate{ true };
	};
}