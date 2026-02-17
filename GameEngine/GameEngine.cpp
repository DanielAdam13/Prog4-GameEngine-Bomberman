#include <stdexcept>
#include <sstream>
#include <iostream>

#include <thread>

#if WIN32
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#endif

#include <SDL3/SDL.h>
//#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "GameEngine.h"
using namespace ge;

#include "Renderer.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "SceneManager.h"

#include "TextComponent.h"

SDL_Window* g_Window{};

void LogSDLVersion(const std::string& message, int major, int minor, int patch)
{
#if WIN32
	std::stringstream ss;
	ss << message << major << "." << minor << "." << patch << "\n";
	OutputDebugString(ss.str().c_str());
#else
	std::cout << message << major << "." << minor << "." << patch << "\n";
#endif
}

#ifdef __EMSCRIPTEN__
#include "emscripten.h"

void LoopCallback(void* arg)
{
	static_cast<GameEngine*>(arg)->RunOneFrame();
}
#endif

// Helper function to identify SDL version errors
void PrintSDLVersion()
{
	LogSDLVersion("Compiled with SDL", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	int version = SDL_GetVersion();
	LogSDLVersion("Linked with SDL ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
	// LogSDLVersion("Compiled with SDL_image ",SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_MICRO_VERSION);
	// version = IMG_Version();
	// LogSDLVersion("Linked with SDL_image ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
	LogSDLVersion("Compiled with SDL_ttf ", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_TTF_MICRO_VERSION);
	version = TTF_Version();
	LogSDLVersion("Linked with SDL_ttf ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
}

void GameEngine::InitializeEngine(const std::filesystem::path& dataPath)
{
	PrintSDLVersion();

	// ---------------------------
	// SDL Initialization
	// ----------------------------
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		SDL_Log("Renderer error: %s", SDL_GetError());
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// ---------------------------
	// SDL Window Creation
	// ----------------------------
	g_Window = SDL_CreateWindow(
		"GameObject/Component Assignment Week 1",
		1024,
		576,
		SDL_WINDOW_OPENGL
	);
	if (g_Window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	// ---------------------------
	// Resource Managerr Initialization
	// ----------------------------
	Renderer::GetInstance().Init(g_Window);
	ResourceManager::GetInstance().Init(dataPath);
}

GameEngine::~GameEngine()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_Window);
	g_Window = nullptr;
	SDL_Quit();
}

void GameEngine::Run(const std::function<void()>& engineStart)
{
	engineStart();

	// Local for Run
	constexpr auto targetFrameRate{ std::chrono::duration<float>(1.f / 60.f) };
	auto lastTime{ std::chrono::high_resolution_clock::now() };
	float fpsTimer{ 0.f };
	int frameCount{ 0 };

	// MAIN GAME LOOP
#ifndef __EMSCRIPTEN__
	while (!m_Quit)
	{
		const auto frameStartTime{ std::chrono::high_resolution_clock::now() };

		m_DeltaTime = std::chrono::duration<float>(frameStartTime - lastTime).count();
		lastTime = frameStartTime;

		RunOneFrame();

		ComputeFPS(fpsTimer, frameCount);

		const auto frameEndTime{ std::chrono::high_resolution_clock::now() };
		const auto frameDuration{ frameEndTime - frameStartTime };

		if (frameDuration < targetFrameRate)
			std::this_thread::sleep_for(targetFrameRate - frameDuration);
	}
#else
	emscripten_set_main_loop_arg(&LoopCallback, this, 0, true);
#endif
}

void GameEngine::RunOneFrame()
{
	m_Quit = !InputManager::GetInstance().ProcessInput();

	m_FrameLag += m_DeltaTime;
	while (m_FrameLag >= m_FixedTimeStep)
	{
		SceneManager::GetInstance().FixedUpdate(m_FixedTimeStep);
		m_FrameLag -= m_FixedTimeStep;
	}

	SceneManager::GetInstance().Update(m_DeltaTime);

	// wow!
	/*SceneManager::GetInstance().GetCurrentScene()->FindObjectByName("GO_TextObject")->
		GetComponent<TextComponent>()->SetText(std::to_string(m_CurrentFPS));*/

	Renderer::GetInstance().Render();
}

void GameEngine::ComputeFPS(float& fpsTimer, int& frameCount)
{
	fpsTimer += m_DeltaTime;
	++frameCount;

	if (fpsTimer >= 1.f)
	{
		m_CurrentFPS = static_cast<float>(frameCount) / fpsTimer;

		frameCount = 0;
		fpsTimer = 0.f;
	}
}
