#include "GameEngine.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "CollisionSystem.h"
#include "IGameApplication.h"

// Services:
#include "Services/ServiceLocator.h"
#include "Services/InputManager.h"
#include "Services/SoundSystem.h"
#include "Services/NullSoundSystem.h"

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

using namespace ge;

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

struct EmscriptenLoopArg {
	GameEngine* engine;
	IGameApplication* app;
};

void LoopCallback(void* arg)
{
	auto* loopArg = static_cast<EmscriptenLoopArg*>(arg);
	loopArg->engine->RunOneFrame(*loopArg->app);
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
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		SDL_Log("Renderer error: %s", SDL_GetError());
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// ---------------------------
	// SDL Window Creation
	// ----------------------------
	static constexpr int windowX{ 800 };
	static constexpr int windowY{ 800 };
	g_Window = SDL_CreateWindow(
		"GameObject/Component Assignment Week 1",
		windowX,
		windowY,
		SDL_WINDOW_OPENGL 
		//| SDL_WINDOW_RESIZABLE
	);
	if (g_Window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	// ---------------------------
	// Resource Managerr Initialization
	// ----------------------------
	Renderer::GetInstance().Init(g_Window, windowX, windowY);
	ResourceManager::GetInstance().Init(dataPath);

	// ---------------------------
	// Service Locator -> Input and Sound Initialization
	// ----------------------------
	ServiceLocator::RegisterInputManager(std::make_unique<ge::InputManager>());
	ServiceLocator::RegisterSoundSystem(std::make_unique<ge::NullSoundSystem>());
}

GameEngine::~GameEngine()
{
	ServiceLocator::Shutdown();
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_Window);
	g_Window = nullptr;
	SDL_Quit();
}

void GameEngine::Run(IGameApplication& app)
{
	app.Load(); // App Load

	// MAIN GAME LOOP
#ifndef __EMSCRIPTEN__
	while (!m_Quit)
	{
		RunOneFrame(app);
	}
#else
	static EmscriptenLoopArg loopArg{ this, &app };
	emscripten_set_main_loop_arg(&LoopCallback, &loopArg, 0, true);
#endif
}

void GameEngine::RunOneFrame(IGameApplication& app)
{
	const auto frameStartTime{ std::chrono::high_resolution_clock::now() };

	// ---- Delta Time calculation ----
	if (m_FirstFrame)
	{
		m_LastTime = frameStartTime;
		m_FirstFrame = false;
	}

	m_DeltaTime = std::chrono::duration<float>(frameStartTime - m_LastTime).count();
	m_LastTime = frameStartTime;

	// ---- MAIN FRAME LOGIC ----
#pragma region MainFrame
	m_Quit = !ge::ServiceLocator::GetInputManager().ProcessInput(m_DeltaTime);

	m_FrameLag += m_DeltaTime;
	while (m_FrameLag >= m_FixedTimeStep)
	{
		SceneManager::GetInstance().FixedUpdate(m_FixedTimeStep);
		app.FixedUpdate(m_FixedTimeStep); // App FixedUpdate
		m_FrameLag -= m_FixedTimeStep;
	}

	app.Update(m_DeltaTime); // App Update -> Game State changes specific for Bomberman
	CollisionSystem::GetInstance().UpdateCollision();
	SceneManager::GetInstance().Update(m_DeltaTime);
	Renderer::GetInstance().Render();
#pragma endregion

	// ---- Frame end ----
	const auto frameEndTime{ std::chrono::high_resolution_clock::now() };
	const auto frameDuration{ frameEndTime - frameStartTime };

	// Thread sleep so target frame rate is matched
	if (frameDuration < targetFrameRate)
		std::this_thread::sleep_for(targetFrameRate - frameDuration);
}
