#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "GameEngine.h"

#include <filesystem>
namespace fs = std::filesystem;

#include <utility>

#include "SceneManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "Image.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Transform.h"

#include "TextComponent.h"

using namespace ge;

static void LoadScenes()
{
	Scene& scene{ SceneManager::GetInstance().CreateScene() };

	// Texture Resources
	auto backgroundTexture{ ResourceManager::GetInstance().LoadTexture("background.png") };
	auto daeTexture{ ResourceManager::GetInstance().LoadTexture("logo.png") };

	auto backgroundGO = std::make_unique<GameObject>("GO_Background");
	backgroundGO->AddComponent<Image>()->SetTexture(backgroundTexture);
	scene.Add(std::move(backgroundGO));
	

	auto daeLogoGO = std::make_unique<GameObject>("GO_DaeLogo");
	daeLogoGO->AddComponent<Image>()->SetTexture(daeTexture);
	scene.Add(std::move(daeLogoGO));

	auto windowSize{ Renderer::GetInstance().GetWindowSize() };

	// For test purposes
	auto daeLogo{ scene.FindObjectByName("GO_DaeLogo") }; 
	if (daeLogo)
		daeLogo->GetComponent<Transform>()->SetPosition({ windowSize.first / 2, windowSize.second / 2, 0.f });

	auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	
	auto textGO = std::make_unique<GameObject>("GO_TextObject");
	textGO->AddComponent<TextComponent>("AHAHA", font, SDL_Color{128, 0, 128, 255});
	scene.Add(std::move(textGO));
}

int main(int, char* [])
{
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./resources/";
	if (!fs::exists(data_location))
		data_location = "../resources/";
#endif
	GameEngine engine(data_location);
	engine.Run(LoadScenes);
	return 0;
}

