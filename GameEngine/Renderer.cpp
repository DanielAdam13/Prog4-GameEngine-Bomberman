#include "Renderer.h"
#include "SceneManager.h"
#include "Texture2D.h"
#include "EngineEvents.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

using namespace ge;

void Renderer::Init(SDL_Window* window, const int windowX, const int windowY)
{
	m_Window = window;

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

#if defined(__EMSCRIPTEN__)
	m_Renderer = SDL_CreateRenderer(window, nullptr);
#else
	m_Renderer = SDL_CreateRenderer(window, nullptr);
#endif

	if (m_Renderer == nullptr)
	{
		std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	m_CurrentWindowSize = { windowX, windowY };
	m_ConstantDesignSize = { static_cast<float>(windowX), static_cast<float>(windowY) };

	// ---- Initialize ImGui ----
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#if __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;
#endif

	ImGui_ImplSDL3_InitForSDLRenderer(window, m_Renderer);
	ImGui_ImplSDLRenderer3_Init(m_Renderer);
}

void Renderer::Render() const
{
	// Render ImGui
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Render SDL
	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_Renderer);

	SceneManager::GetInstance().Render(); // ImGui is rendered inside an idividual scene

	// DRAW ImGui on top of SDL window
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_Renderer);

	SDL_RenderPresent(m_Renderer);
}

void Renderer::Destroy()
{
	// Destroy ImGui
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	// Destroy SDL Renderer
	if (m_Renderer != nullptr)
	{
		SDL_DestroyRenderer(m_Renderer);
		m_Renderer = nullptr;
	}
}

void Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
{
	SDL_FRect dst{};
	float w{};
	float h{};
	SDL_GetTextureSize(texture.GetSDLTexture(), &w, &h);
	dst.x = x * m_RenderScale.first;
	dst.y = y * m_RenderScale.second;
	dst.w = w * m_RenderScale.first;
	dst.h = h * m_RenderScale.second;
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_FRect dst{};
	dst.x = x * m_RenderScale.first;
	dst.y = y * m_RenderScale.second;
	dst.w = width * m_RenderScale.first;
	dst.h = height * m_RenderScale.second;
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void Renderer::RenderRectOutline(float x, float y, float w, float h, const SDL_Color& color) const
{
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	SDL_FRect r{
		x * m_RenderScale.first,
		y * m_RenderScale.second,
		w * m_RenderScale.first,
		h * m_RenderScale.second
	};
	SDL_RenderRect(m_Renderer, &r);
}

void ge::Renderer::RenderTextureRegion(const Texture2D& texture, const SDL_FRect& srcRect, float x, float y, float w, float h) const
{
	SDL_FRect dst{
		x * m_RenderScale.first,
		y * m_RenderScale.second,
		w * m_RenderScale.first,
		h * m_RenderScale.second
	};
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), &srcRect, &dst);
}

std::pair<int, int> Renderer::GetWindowSize() const
{
	int w{}, h{};
	SDL_GetWindowSize(m_Window, &w, &h);
	return std::pair<int, int>(w, h);
}

void Renderer::SetWindowSize(std::pair<int, int> newSize)
{
	if (newSize.first < 50 && newSize.second < 50)
		return;

	m_CurrentWindowSize = newSize;
	m_RenderScale = { float(newSize.first) / m_ConstantDesignSize.first,
		float(newSize.second) / m_ConstantDesignSize.second };

	SDL_SetWindowSize(m_Window, newSize.first, newSize.second);
	m_OnScreenResizeEvent.NotifyObservers(EngineEventId::WINDOW_RESIZED, nullptr);
}

Subject& ge::Renderer::GetOnScreenResizeEvent() noexcept
{
	return m_OnScreenResizeEvent;
}

std::pair<float, float> ge::Renderer::GetWindowDesignSize() const noexcept
{
	return m_ConstantDesignSize;
}
