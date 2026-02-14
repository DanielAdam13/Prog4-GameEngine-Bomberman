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

using namespace ge;

static void LoadScenes()
{
	Scene& scene{ ge::SceneManager::GetInstance().CreateScene() };

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

	auto daeLogo{ scene.FindObjectByName("GO_DaeLogo") }; 
	if (daeLogo)
		daeLogo->GetComponent<Transform>()->SetPosition({ windowSize.first / 2, windowSize.second / 2, 0.f });

	/*auto go = std::make_unique<ge::GameObject>();
	go->SetTexture("background.png");
	scene.Add(std::move(go));

	go = std::make_unique<ge::GameObject>();
	go->SetTexture("logo.png");
	go->SetPosition(358, 180);
	scene.Add(std::move(go));

	auto font = ge::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto to = std::make_unique<ge::TextObject>("Programming 4 Assignment", font);
	to->SetColor({ 255, 255, 0, 255 });
	to->SetPosition(292, 20);
	scene.Add(std::move(to));*/
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

