#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "GameEngine.h"
#include "BombermanGame.h"

#include <Services/ServiceLocator.h>
#include "Services/SDLSoundSystem.h"
#include <Services/LoggingSoundService.h>

#include <memory>
#include <filesystem>

using namespace ge;
using namespace bombGame;

int main(int, char* [])
{
#if __EMSCRIPTEN__
	std::filesystem::path data_location = "";
#else
	std::filesystem::path data_location = "./resources/";
	if (!std::filesystem::exists(data_location))
		data_location = "../resources/";
#endif
	GameEngine::GetInstance().InitializeEngine(data_location);

	// Select Sound System:
#if _DEBUG
	ge::ServiceLocator::RegisterSoundSystem(std::make_unique<LoggingSoundSystem>(std::make_unique<SDLSoundSystem>()));
#else 
	ge::ServiceLocator::RegisterSoundSystem(std::make_unique<SDLSoundSystem>());
#endif

	BombermanGame Game{};

	GameEngine::GetInstance().Run(Game.LoadGame);
	return 0;
}
