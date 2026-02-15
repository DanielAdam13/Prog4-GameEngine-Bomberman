#include <SDL3/SDL.h>
#include "Texture2D.h"
#include "Renderer.h"
#include <stdexcept>

using namespace ge;

SDL_Texture* Texture2D::GetSDLTexture() const
{
    return m_pTexture;
}

Texture2D::Texture2D(const std::string& fullPath)
{
    SDL_Surface* surface = SDL_LoadPNG(fullPath.c_str());
    if (!surface)
    {
        throw std::runtime_error(
            std::string("Failed to load PNG: ") + SDL_GetError()
        );
    }

    m_pTexture = SDL_CreateTextureFromSurface(
        Renderer::GetInstance().GetSDLRenderer(),
        surface
    );

    SDL_DestroySurface(surface);

    if (!m_pTexture)
    {
        throw std::runtime_error(
            std::string("Failed to create texture from surface: ") + SDL_GetError()
        );
    }
}

Texture2D::Texture2D(SDL_Texture* texture) 
    : m_pTexture{ texture }
{
    assert(m_pTexture != nullptr);
}

Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_pTexture);
}

glm::vec2 Texture2D::GetSize() const
{
    float w{}, h{};
    SDL_GetTextureSize(m_pTexture, &w, &h);
    return { w, h };
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (this == &other)
        return *this;

    if (m_pTexture)
        SDL_DestroyTexture(m_pTexture);

    m_pTexture = other.m_pTexture;

    other.m_pTexture = nullptr;

    return *this;
}
