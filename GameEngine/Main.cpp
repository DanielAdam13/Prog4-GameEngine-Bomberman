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

#include "InputManager.h"
#include "Commands/MoveCommand.h"
//#include "Commands/MoveStickCommand.h"
#include "Commands/DamageCommand.h"
#include "Commands/ScoreCommand.h"
#include "Commands/ConditionalCommand.h"

#include "Player.h"
#include "Achievements.h"
#include "HealthObserver.h"
#include "ScoreObserver.h"
#include "ObservableSubject.h"
#include <string>

using namespace ge;
using namespace bombGame;

// TEMPORARY since my intention is for these to live over scenes, on the Game level
std::unique_ptr<AchievementsObserver> g_AchievementsObserver{};
std::unique_ptr<HealthObserver> g_HealthObserver{};
std::unique_ptr<Player> g_Player1{};
std::unique_ptr<Player> g_Player2{};
std::unique_ptr<ScoreObserver> g_ScoreObserver{};

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

	const auto font{ ResourceManager::GetInstance().LoadFont("Lingua.otf", 28) };
	const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
	constexpr SDL_Color color1{ SDL_Color{20, 100, 200, 255} };
	constexpr SDL_Color color2{ SDL_Color{220, 100, 50, 255} };

	// 1. Player Initialization
	const auto playerTexture{ ResourceManager::GetInstance().LoadTexture("I_Player_Bomberman.png") };
	const auto balloonTexture{ ResourceManager::GetInstance().LoadTexture("I_Balloon_Bomberman.png") };

	auto player1GO = std::make_unique<GameObject>("GO_Player1");
	g_Player1 = std::make_unique<Player>( player1GO.get(), playerTexture, 
		120.f, 3, glm::vec3{ 250.f, 350.f, 0.f }, glm::vec3{ 2.5f, 2.5f, 2.5f } );

	auto player2GO = std::make_unique<GameObject>("GO_Player2");
	g_Player2 = std::make_unique<Player>(player2GO.get(), balloonTexture, 
		240.f, 3, glm::vec3{ 200.f, 150.f, 0.f }, glm::vec3{ 2.f, 2.f, 2.f });

	// Apply observer pattern:
	g_AchievementsObserver = std::make_unique<AchievementsObserver>();
	g_Player1->GetDeadEvent().AddObserver(g_AchievementsObserver.get());
	g_Player2->GetDeadEvent().AddObserver(g_AchievementsObserver.get());

	g_HealthObserver = std::make_unique<HealthObserver>();
	g_Player1->GetDamageEvent().AddObserver(g_HealthObserver.get());
	g_Player1->GetDeadEvent().AddObserver(g_HealthObserver.get());
	g_Player2->GetDamageEvent().AddObserver(g_HealthObserver.get());
	g_Player2->GetDeadEvent().AddObserver(g_HealthObserver.get());

	g_ScoreObserver = std::make_unique<ScoreObserver>();
	g_Player1->GetScoreEvent().AddObserver(g_ScoreObserver.get());
	g_Player2->GetScoreEvent().AddObserver(g_ScoreObserver.get());

	// 2. Health Displays:
	auto p1HealthDisplay = std::make_unique<GameObject>("GO_FirstPlayerDisplay");
	p1HealthDisplay->AddComponent<TextComponent>(p1HealthDisplay.get(),
		std::string(player1GO->GetName() + " HP: " + std::to_string(g_Player1->GetPlayerHealth())), font, color1);
	p1HealthDisplay->GetComponent<Transform>()->SetLocalPosition(glm::vec3{
		windowSize.first * 0.05f, windowSize.second * 0.9f, 0.f });

	auto p2HealthDisplay = std::make_unique<GameObject>("GO_SecondPlayerDisplay");
	p2HealthDisplay->AddComponent<TextComponent>(p2HealthDisplay.get(),
		std::string(player2GO->GetName() + " HP: " + std::to_string(g_Player2->GetPlayerHealth())), font, color2);
	p2HealthDisplay->GetComponent<Transform>()->SetLocalPosition(glm::vec3{ 
		windowSize.first * 0.75f, windowSize.second * 0.9f, 0.f });

	// 3. Score Displays:
	auto p1ScoreDisplay = std::make_unique<GameObject>("GO_FirstPlayerScore");
	p1ScoreDisplay->AddComponent<TextComponent>(p1ScoreDisplay.get(),
		"Score: 0", font, color1);
	p1ScoreDisplay->GetComponent<Transform>()->SetLocalPosition(glm::vec3{
		windowSize.first * 0.05f, windowSize.second * 0.8f, 0.f });

	auto p2ScoreDisplay = std::make_unique<GameObject>("GO_SecondPlayerScore");
	p2ScoreDisplay->AddComponent<TextComponent>(p2ScoreDisplay.get(),
		"Score: 0", font, color2);
	p2ScoreDisplay->GetComponent<Transform>()->SetLocalPosition(glm::vec3{
		windowSize.first * 0.75f, windowSize.second * 0.8f, 0.f });

	// 4. Register player health and score displays to HEALTH and SCORE observers
	g_HealthObserver->RegisterPlayer(player1GO.get(), p1HealthDisplay->GetComponent<TextComponent>());
	g_HealthObserver->RegisterPlayer(player2GO.get(), p2HealthDisplay->GetComponent<TextComponent>());

	g_ScoreObserver->RegisterPlayer(player1GO.get(), p1ScoreDisplay->GetComponent<TextComponent>());
	g_ScoreObserver->RegisterPlayer(player2GO.get(), p2ScoreDisplay->GetComponent<TextComponent>());


	// 5. Command Binding to two players
#pragma region CommandBinding
	auto& input{ InputManager::GetInstance() };

	// Player speeds
	const float firstPlayerSpeed{ g_Player1->GetPlayerSpeed()};
	const float secondPlayerSpeed{ g_Player2->GetPlayerSpeed() };

	auto deathConditionLambda1{ [&]() -> bool { return !g_Player1->IsPlayerDead(); } };
	auto deathConditionLambda2{ [&]() -> bool { return !g_Player2->IsPlayerDead(); } };
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

	InputTestScene.Add(std::move(p1HealthDisplay));
	InputTestScene.Add(std::move(p2HealthDisplay));

	InputTestScene.Add(std::move(p1ScoreDisplay));
	InputTestScene.Add(std::move(p2ScoreDisplay));

}
