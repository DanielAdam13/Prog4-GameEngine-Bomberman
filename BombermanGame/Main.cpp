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

#include "Components/Image.h"
#include "Components/Transform.h"
#include "Components/TextComponent.h"
#include "Components/FPSComponent.h"
#include "Components/RotatorComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"

#include "Components/PlayerComponent.h"
#include "Components/HealthDisplayComponent.h"
#include "Components/ScoreDisplayComponent.h"

//#include "InputManager.h"
#include "Commands/MoveCommand.h"
#include "Commands/DamageCommand.h"
#include "Commands/ScoreCommand.h"
#include "Commands/ConditionalCommand.h"

#include "Achievements.h"

#include "ObservableSubject.h"
#include <string>

#include "Services/ServiceLocator.h"
#include "Services/SDLSoundSystem.h"
#include "Services/LoggingSoundService.h"
#include "Services/InputManager.h"

using namespace ge;
using namespace bombGame;

// TEMPORARY since my intention is for these to live over scenes, on the Game level
std::unique_ptr<AchievementsManager> g_AchievementsObserver{};

void InitializeFirstScene();
void InitializeImGuiExercisesScene();
void InitializeMainPlayersScene();

static void LoadScenes()
{
	InitializeFirstScene();
	//InitializeImGuiExercisesScene();
	InitializeMainPlayersScene();
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

	// Initialize Services:
#if _DEBUG
	ge::ServiceLocator::RegisterSoundSystem(std::make_unique<LoggingSoundSystem>(std::make_unique<SDLSoundSystem>()));
#else 
	ge::ServiceLocator::RegisterSoundSystem(std::make_unique<SDLSoundSystem>());
#endif

	GameEngine::GetInstance().Run(LoadScenes);
	return 0;
}

void InitializeFirstScene()
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

	const bool TEST{ textGO->HasComponent<TextComponent>() };

	scene.Add(std::move(textGO));

	auto prog4AssingGO = std::make_unique<GameObject>("GO_P4AssignmentText");
	prog4AssingGO->AddComponent<TextComponent>(prog4AssingGO.get(), 
		(TEST) ? "Programming 4 Assignment" : "false", font, color);

	prog4AssingGO->GetComponent<Transform>()->SetLocalPosition({
		windowSize.first / 2 - prog4AssingGO->GetComponent<TextComponent>()->GetTextureSize().x / 2,
		0.f, 0.f });
	scene.Add(std::move(prog4AssingGO));

	const auto tutFont{ ResourceManager::GetInstance().LoadFont("Lingua.otf", 20) };

	auto tutorial1GO = std::make_unique<GameObject>("GO_TutorialText1");
	tutorial1GO->AddComponent<TextComponent>(tutorial1GO.get(),
		"Use the D-Pad to move the Balloon, X to inflict damage, A to collect", tutFont, color);
	auto tutorial2GO = std::make_unique<GameObject>("GO_TutorialText1");
	tutorial2GO->AddComponent<TextComponent>(tutorial2GO.get(),
		"Use WASD to move the BomberMan, X to inflict damage, E to collect", tutFont, color);

	tutorial1GO->GetComponent<Transform>()->SetLocalPosition(glm::vec3{ 0.f, windowSize.second / 10, 0.f });
	tutorial2GO->GetComponent<Transform>()->SetLocalPosition(glm::vec3{ 0.f, windowSize.second / 6, 0.f });


	scene.Add(std::move(tutorial1GO));
	scene.Add(std::move(tutorial2GO));


	// WEEK 2 - Rotation
	/*const auto balloonTexture{ ResourceManager::GetInstance().LoadTexture("I_Balloon_Bomberman.png") };

	auto BalloonGO1 = std::make_unique<GameObject>("GO_Ballon_0");
	BalloonGO1->AddComponent<Image>(BalloonGO1.get())->SetTexture(balloonTexture);
	auto ballonTransform{ BalloonGO1->GetComponent<Transform>() };

	ballonTransform->SetLocalScale({ 2.f, 2.f, 1.f });
	ballonTransform->SetLocalPosition(ballonTransform->GetWorldPosition() + glm::vec3{ 250.f, 350.f, 0.f });
	BalloonGO1->AddComponent<Rotator>(BalloonGO1.get(), 360.f, 30.f);
	scene.Add(std::move(BalloonGO1));

	auto BalloonGO2 = std::make_unique<GameObject>("GO_Ballon_1");
	BalloonGO2->AddComponent<Image>(BalloonGO2.get())->SetTexture(balloonTexture);
	BalloonGO2->SetParent(scene.FindObjectByID(4), false);
	BalloonGO2->AddComponent<Rotator>(BalloonGO2.get(), -360.f, 100.f);
	scene.Add(std::move(BalloonGO2));*/
}

void InitializeImGuiExercisesScene()
{
	Scene& ImGuiExercisesScene{ SceneManager::GetInstance().CreateScene() };
	ImGuiExercisesScene.AddImGuiScene(std::make_unique<Exercise1ImGui>());
}

void InitializeMainPlayersScene()
{
	Scene& InputTestScene{ SceneManager::GetInstance().CreateScene() };

	// Resources:
	const auto font{ ResourceManager::GetInstance().LoadFont("Lingua.otf", 28) };
	const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
	constexpr SDL_Color color1{ SDL_Color{20, 100, 200, 255} };
	constexpr SDL_Color color2{ SDL_Color{220, 100, 50, 255} };

	const auto playerTexture{ ResourceManager::GetInstance().LoadTexture("I_Player_Bomberman.png") };
	const auto balloonTexture{ ResourceManager::GetInstance().LoadTexture("I_Balloon_Bomberman.png") };

	// 1. Player Initialization
	auto player1GO = std::make_unique<GameObject>("GO_Player1");
	player1GO->AddComponent<ge::Image>(player1GO.get())->SetTexture(playerTexture);
	player1GO->GetComponent<ge::Transform>()->SetLocalPosition({ 250.f, 350.f, 0.f });
	player1GO->GetComponent<ge::Transform>()->SetLocalScale({ 2.5f, 2.5f, 2.5f });
	player1GO->AddComponent<HealthComponent>(player1GO.get(), 3);
	player1GO->AddComponent<ScoreComponent>(player1GO.get(), 0);
	player1GO->AddComponent<PlayerComponent>(player1GO.get(), 120.f);

	auto player2GO = std::make_unique<GameObject>("GO_Player2");
	player2GO->AddComponent<ge::Image>(player2GO.get())->SetTexture(balloonTexture);
	player2GO->GetComponent<ge::Transform>()->SetLocalPosition({ 200.f, 150.f, 0.f });
	player2GO->GetComponent<ge::Transform>()->SetLocalScale({ 2.f, 2.f, 2.f });
	player2GO->AddComponent<HealthComponent>(player2GO.get(), 3);
	player2GO->AddComponent<ScoreComponent>(player2GO.get(), 0);
	player2GO->AddComponent<PlayerComponent>(player2GO.get(), 240.f);


	// 2. Health Displays:
	auto p1HealthDisplayGO = std::make_unique<GameObject>("GO_P1HealthDisplay");
	p1HealthDisplayGO->AddComponent<ge::TextComponent>(p1HealthDisplayGO.get(), "", font, color1);
	p1HealthDisplayGO->AddComponent<HealthDisplayComponent>(p1HealthDisplayGO.get(), player1GO.get());
	p1HealthDisplayGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.05f, windowSize.second * 0.9f, 0.f });

	auto p2HealthDisplayGO = std::make_unique<GameObject>("GO_P2HealthDisplay");
	p2HealthDisplayGO->AddComponent<ge::TextComponent>(p2HealthDisplayGO.get(), "", font, color2);
	p2HealthDisplayGO->AddComponent<HealthDisplayComponent>(p2HealthDisplayGO.get(), player2GO.get());
	p2HealthDisplayGO->GetComponent<Transform>()->SetLocalPosition({
		windowSize.first * 0.75f, windowSize.second * 0.9f, 0.f });

	// 3. Score Displays:
	auto p1ScoreDisplayGO = std::make_unique<GameObject>("GO_FirstPlayerScore");
	p1ScoreDisplayGO->AddComponent<TextComponent>(p1ScoreDisplayGO.get(), "Score: 0", font, color1);
	p1ScoreDisplayGO->AddComponent<ScoreDisplayComponent>(p1ScoreDisplayGO.get(), player1GO.get());
	p1ScoreDisplayGO->GetComponent<Transform>()->SetLocalPosition({
		windowSize.first * 0.05f, windowSize.second * 0.8f, 0.f });

	auto p2ScoreDisplayGO = std::make_unique<GameObject>("GO_SecondPlayerScore");
	p2ScoreDisplayGO->AddComponent<TextComponent>(p2ScoreDisplayGO.get(), "Score: 0", font, color2);
	p2ScoreDisplayGO->AddComponent<ScoreDisplayComponent>(p2ScoreDisplayGO.get(), player2GO.get());
	p2ScoreDisplayGO->GetComponent<Transform>()->SetLocalPosition({
		windowSize.first * 0.75f, windowSize.second * 0.8f, 0.f });

	// 4. Command Binding to two players
#pragma region CommandBinding
	auto& input{ ge::ServiceLocator::GetInputManager() };

	auto* p1PlayerCompRaw{ player1GO.get()->GetComponent<PlayerComponent>() };
	auto* p2PlayerCompRaw{ player2GO.get()->GetComponent<PlayerComponent>() };

	// Player speeds
	auto deathConditionLambda1{ [p1PlayerCompRaw]() -> bool { return p1PlayerCompRaw && p1PlayerCompRaw->IsAlive(); } };
	auto deathConditionLambda2{ [p2PlayerCompRaw]() -> bool { return p2PlayerCompRaw && p2PlayerCompRaw->IsAlive(); } };

	const float firstPlayerSpeed{ p1PlayerCompRaw->GetSpeed() };
	const float secondPlayerSpeed{ p2PlayerCompRaw->GetSpeed() };

	// First player:
	input.BindKeyboardCommand(SDL_SCANCODE_W, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player1GO.get(), 
			glm::vec3{ 0.f, -1.f, 0.f }, firstPlayerSpeed),
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_A, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player1GO.get(), 
			glm::vec3{ -1.f, 0.f, 0.f }, firstPlayerSpeed),
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_S, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player1GO.get(), 
			glm::vec3{ 0.f, 1.f, 0.f }, firstPlayerSpeed),
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_D, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player1GO.get(), 
			glm::vec3{ 1.f, 0.f, 0.f }, firstPlayerSpeed),
			deathConditionLambda1));

	// Player 1 command but target is Player 2 and uses a condition checking if player 1 is dead
	input.BindKeyboardCommand(SDL_SCANCODE_X, InputManager::InputTrigger::Up,
		std::make_unique<ConditionalCommand>(std::make_unique<DamageCommand>(player2GO.get(), 1), 
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_E, InputManager::InputTrigger::Up,
		std::make_unique<ConditionalCommand>(std::make_unique<ScoreCommand>(player1GO.get(), 10),
			deathConditionLambda1));

	// Second player:
	input.BindControllerCommand(ControllerButton::DpadUp, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player2GO.get(), 
			glm::vec3{ 0.f, -1.f, 0.f }, secondPlayerSpeed),
			deathConditionLambda2));
	input.BindControllerCommand(ControllerButton::DpadLeft, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player2GO.get(),
			glm::vec3{ -1.f, 0.f, 0.f }, secondPlayerSpeed),
			deathConditionLambda2));
	input.BindControllerCommand(ControllerButton::DpadDown, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player2GO.get(),
			glm::vec3{ 0.f, 1.f, 0.f }, secondPlayerSpeed),
			deathConditionLambda2));
	input.BindControllerCommand(ControllerButton::DpadRight, InputManager::InputTrigger::Pressed,
		std::make_unique<ConditionalCommand>(std::make_unique<MoveCommand>(player2GO.get(),
			glm::vec3{ 1.f, 0.f, 0.f }, secondPlayerSpeed),
			deathConditionLambda2));

	// Player 2 command but target is Player 1 and uses a condition checking if player 2 is dead
	input.BindControllerCommand(ControllerButton::X, InputManager::InputTrigger::Up,
		std::make_unique<ConditionalCommand>(std::make_unique<DamageCommand>(player1GO.get(), 1),
			deathConditionLambda2));
	input.BindControllerCommand(ControllerButton::A, InputManager::InputTrigger::Up,
		std::make_unique<ConditionalCommand>(std::make_unique<ScoreCommand>(player2GO.get(), 10),
			deathConditionLambda2));
#pragma endregion

	InputTestScene.Add(std::move(player1GO));
	InputTestScene.Add(std::move(player2GO));

	InputTestScene.Add(std::move(p1HealthDisplayGO));
	InputTestScene.Add(std::move(p2HealthDisplayGO));

	InputTestScene.Add(std::move(p1ScoreDisplayGO));
	InputTestScene.Add(std::move(p2ScoreDisplayGO));

}
