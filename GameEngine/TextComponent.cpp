#include "TextComponent.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>

#include "Font.h"
#include "Renderer.h"
#include <SDL3/SDL_render.h>
#include "Texture2D.h"

using namespace ge;

TextComponent::TextComponent(const std::string& text, Font* font, const SDL_Color& color)
	:m_Text{ text },
	m_pTextFont{ font },
	m_TextColor{ color },
	m_TextTexture{ nullptr },
	m_ShouldUpdate{ true }
{
	UpdateTextureForText();
}

void TextComponent::UpdateComponent(float)
{
	if (m_ShouldUpdate)
	{
		UpdateTextureForText();

		m_ShouldUpdate = false;
	}
}

void TextComponent::RenderComponent(const glm::vec3& transformPos) const
{
	if (m_TextTexture != nullptr)
	{
		Renderer::GetInstance().RenderTexture(*m_TextTexture.get(), transformPos.x, transformPos.y);
	}
}

void TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_ShouldUpdate = true;
}

void TextComponent::SetColor(const SDL_Color& color)
{
	m_TextColor = color;
	m_ShouldUpdate = true;
}

glm::vec2 TextComponent::GetTextureSize() const noexcept
{
	return m_TextTexture.get()->GetSize();
}

void TextComponent::UpdateTextureForText()
{
	const auto textSurface = TTF_RenderText_Blended(m_pTextFont->GetFont(), m_Text.c_str(), m_Text.length(), m_TextColor);
	if (textSurface == nullptr)
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());

	// Create Resource in Renderer
	auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), textSurface);
	if (texture == nullptr)
	{
		throw std::runtime_error(std::string("Create text texture from textSurface failed: ") + SDL_GetError());
	}
	SDL_DestroySurface(textSurface);

	// Assign a new SDL Texture via the Texture2D wrapper
	if (!m_TextTexture)
	{
		m_TextTexture = std::make_unique<Texture2D>(texture);
	}
	else
	{
		*m_TextTexture = Texture2D(texture); // Move assingment
	}
}
