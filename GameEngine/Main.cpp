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
#include "ResourceManager.h"
#include "Renderer.h"

#include "Image.h"
#include "Transform.h"
#include "TextComponent.h"
#include "FPSComponent.h"

using namespace ge;

static void LoadScenes()
{
	Scene& scene{ SceneManager::GetInstance().CreateScene() };

	// Texture Resources
	const auto backgroundTexture{ ResourceManager::GetInstance().LoadTexture("background.png") };
	const auto daeTexture{ ResourceManager::GetInstance().LoadTexture("logo.png") };

	auto backgroundGO = std::make_unique<GameObject>("GO_Background");
	backgroundGO->AddComponent<Image>(backgroundGO.get())->SetTexture(backgroundTexture);
	scene.Add(std::move(backgroundGO));


	auto daeLogoGO = std::make_unique<GameObject>("GO_DaeLogo");
	daeLogoGO->AddComponent<Image>(daeLogoGO.get())->SetTexture(daeTexture);
	scene.Add(std::move(daeLogoGO));

	const auto windowSize{ Renderer::GetInstance().GetWindowSize() };

	// For test purposes
	auto daeLogo{ scene.FindObjectByName("GO_DaeLogo") }; 
	if (daeLogo)
	{
		auto daeImageComponent{ daeLogo->GetComponent<Image>() };
		glm::vec2 imageSize{ daeImageComponent->GetTexture()->GetSize() };
		daeLogo->GetComponent<Transform>()->SetLocalPosition(
			{ windowSize.first / 2 - imageSize.x / 2, 
			windowSize.second / 2 - imageSize.y / 2, 
			0.f });
	}

	const auto font{ ResourceManager::GetInstance().LoadFont("Lingua.otf", 36) };
	constexpr SDL_Color color{ SDL_Color{128, 128, 128, 255} };
	
	auto textGO = std::make_unique<GameObject>("GO_TextObject");
	textGO->AddComponent<TextComponent>(textGO.get(), "0.00 FPS", font, color);
	textGO->AddComponent<FPSComponent>(textGO.get());

	// TEST Reparenting:
	/*textGO->SetParent(scene.FindObjectByID(1));

	auto textTrans{ textGO->GetComponent<Transform>() };
	textTrans->SetLocalPosition(textTrans->GetWorldPosition() + glm::vec3{100.f, 0.f, 0.f});

	auto parentTrans{ scene.FindObjectByID(1)->GetComponent<Transform>() };
	parentTrans->SetLocalPosition(parentTrans->GetLocalPosition() + glm::vec3{ 0.f, 150.f, 0.f });
	textGO->SetParent(nullptr);

	parentTrans->SetLocalPosition(parentTrans->GetLocalPosition() + glm::vec3{ 0.f, -150.f, 0.f });
	textTrans->SetLocalPosition(textTrans->GetWorldPosition() + glm::vec3{ 250.f, 0.f, 0.f });*/

	const bool TEST{ textGO->HasComponent<TextComponent>() };

	scene.Add(std::move(textGO));

	auto prog4AssingGO = std::make_unique<GameObject>("GO_P4AssText");
	prog4AssingGO->AddComponent<TextComponent>(prog4AssingGO.get(), (TEST) ? "Programming 4 Assignment" : "false", font, color);

	prog4AssingGO->GetComponent<Transform>()->SetLocalPosition({ 
		windowSize.first / 2 - prog4AssingGO->GetComponent<TextComponent>()->GetTextureSize().x / 2,
		0.f, 0.f });
	scene.Add(std::move(prog4AssingGO));


	// WEEK 2 - Rotation
	const auto balloonTexture{ ResourceManager::GetInstance().LoadTexture("I_Balloon_Bomberman.png") };

	auto BalloonGO = std::make_unique<GameObject>("GO_Ballon_0");
	BalloonGO->AddComponent<Image>(BalloonGO.get())->SetTexture(balloonTexture);

	auto ballonTransform{ BalloonGO->GetComponent<Transform>() };
	ballonTransform->SetLocalScale({ 1.f, 1.f, 1.f });
	//ballonTransform->SetLocalPosition(ballonTransform->GetWorldPosition() + glm::vec3{ 100.f, 100.f, 0.f });

	BalloonGO->SetParent(scene.FindObjectByID(1));
	scene.FindObjectByID(1)->GetComponent<Transform>()->SetLocalScale({ 2.f, 2.f, 1.f });
	scene.FindObjectByID(1)->GetComponent<Transform>()->SetLocalPosition({ 0.f, 0.f, 0.f });

	scene.Add(std::move(BalloonGO));
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
	GameEngine::GetInstance().InitializeEngine(data_location);
	GameEngine::GetInstance().Run(LoadScenes);
	return 0;
}

