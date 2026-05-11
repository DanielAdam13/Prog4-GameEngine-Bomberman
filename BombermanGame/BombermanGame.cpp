#include "BombermanGame.h"

#include "SceneManager.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Renderer.h"
#include <Services/ServiceLocator.h>
#include <Services/InputManager.h>
#include <Services/SoundSystem.h>
#include "Services/SoundIds.h"

#include "Components/Image.h"
#include "Components/Transform.h"
#include "Components/TextComponent.h"
#include "Components/FPSComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"

#include "Components/PlayerComponent.h"
#include "Components/HealthDisplayComponent.h"
#include "Components/ScoreDisplayComponent.h"
#include "Components/BombComponent.h"
#include "Components/BombLayerComponent.h"
#include "Components/EnemyComponent.h"

#include "Components/FSMComponent.h"
#include "StateMachine/ChaseState.h"
#include "StateMachine/WanderState.h"
#include "StateMachine/RunState.h"
#include "StateMachine/Transition.h"

#include "Commands/ConditionalCommand.h"
#include "Commands/MoveCommand.h"

#include "Commands/DamageCommand.h"
#include "Commands/ScoreCommand.h"
#include "Commands/LayBombCommand.h"

#include "Components/Colliders.h"
#include "CollisionSystem.h"
#include <Structs.h>

#include "SoundManager.h"
#include "Utils.h"

#include <memory>


ge::SoundSystem* bombGame::BombermanGame::StoredSoundSystem{ nullptr };
bombGame::SoundManager bombGame::BombermanGame::BombermanSoundManager{};

float bombGame::BombermanGame::CurrentBombExplosion{ 2.f };
ge::GameObject bombGame::BombermanGame::CurrentBombTemplate{ "GO_TempBomb" };

bombGame::BombermanGame::BombermanGame()
{
	StoredSoundSystem = &ge::ServiceLocator::GetSoundSystem();
}

bombGame::BombermanGame::~BombermanGame() = default;

void bombGame::BombermanGame::LoadGame()
{
	LoadSound();
	LoadScenes();
	//StoredSoundSystem->Play(SoundIds::BombermanDied, 0.15f);
}

void bombGame::BombermanGame::LoadSound()
{
	// Pre-load sound files:
	StoredSoundSystem->RegisterSound(SoundIds::ExplosionBomb, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomb_explosion.wav"));
	StoredSoundSystem->RegisterSound(SoundIds::BombermanDied, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomberman_killed.wav"));
	StoredSoundSystem->RegisterSound(SoundIds::LayBomb, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomb_lay.wav"));
	StoredSoundSystem->RegisterSound(SoundIds::Pause, ge::ResourceManager::GetInstance().GetFullPath("sounds/pause.wav"));
	StoredSoundSystem->RegisterSound(SoundIds::Powerup, ge::ResourceManager::GetInstance().GetFullPath("sounds/powerup.wav"));
	StoredSoundSystem->RegisterSound(SoundIds::Step_Horizontal, ge::ResourceManager::GetInstance().GetFullPath("sounds/step_horizontal.wav"));
	StoredSoundSystem->RegisterSound(SoundIds::Step_Vertical, ge::ResourceManager::GetInstance().GetFullPath("sounds/step_vertical.wav"));
}

void bombGame::BombermanGame::LoadScenes()
{
	InitializeMainGameplayScene();
}

bombGame::SoundManager& bombGame::BombermanGame::GetSoundManager() noexcept
{
	return BombermanSoundManager;
}

void bombGame::BombermanGame::InitializeMainGameplayScene()
{
	ge::Scene& MainGameplayScene{ ge::SceneManager::GetInstance().CreateScene() };

	// -----------------------------------------------
	// Resources:
	// -----------------------------------------------
	const auto font{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 28) };
	font->SetBold(true);
	ge::Renderer::GetInstance().SetWindowSize(800, 800);
	const auto windowSize{ ge::Renderer::GetInstance().GetWindowSize() };
	//constexpr SDL_Color colorBlack{ SDL_Color{0, 0, 0, 255} };
	constexpr SDL_Color colorRed{ SDL_Color{120, 20, 50, 255} };
	constexpr SDL_Color colorBlue{ SDL_Color{45, 50, 130, 255} };
	const auto tutFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 20) };
	tutFont->SetBold(true);

	const auto playerTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_Player_Bomberman.png") };
	const auto balloonTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_Balloon_Bomberman.png") };
	const auto backgroundTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_PlayField.png") };
	const auto bombTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_Bomb.png") };
	const auto iceEnemyTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_IceEnemy.png") };
	
	// -----------------------------------------------
	// Static objects in scene initialization:
	// -----------------------------------------------
	auto backgroundGO = std::make_unique<ge::GameObject>("GO_Background");
	backgroundGO->AddComponent<ge::Image>(backgroundGO.get())->SetTexture(backgroundTexture);
	backgroundGO->GetComponent<ge::Transform>()->SetLocalScale(3.5f, 3.5f, 1.f);
	backgroundGO->GetComponent<ge::Transform>()->SetLocalPosition(0.f, 50.f, 0.f);
	MainGameplayScene.Add(std::move(backgroundGO));
#if _DEBUG
	auto textGO = std::make_unique<ge::GameObject>("GO_TextObject");
	textGO->AddComponent<ge::TextComponent>(textGO.get(), "0.00 FPS", font, colorRed);
	textGO->AddComponent<ge::FPSComponent>(textGO.get());

	MainGameplayScene.Add(std::move(textGO));
#endif

	auto tutorial1GO = std::make_unique<ge::GameObject>("GO_TutorialText1");
	tutorial1GO->AddComponent<ge::TextComponent>(tutorial1GO.get(),
		"D-Pad to move the Balloon | X damage | A score | B bomb", tutFont, colorRed);
	auto tutorial2GO = std::make_unique<ge::GameObject>("GO_TutorialText1");
	tutorial2GO->AddComponent<ge::TextComponent>(tutorial2GO.get(),
		"WASD to move the BomberMan | X damage | E score | SPACE bomb", tutFont, colorBlue);

	tutorial1GO->GetComponent<ge::Transform>()->SetLocalPosition(glm::vec3{ 0.f, windowSize.second / 10, 0.f });
	tutorial2GO->GetComponent<ge::Transform>()->SetLocalPosition(glm::vec3{ 0.f, windowSize.second / 6, 0.f });

	MainGameplayScene.Add(std::move(tutorial1GO));
	MainGameplayScene.Add(std::move(tutorial2GO));

	// Collision
	ge::CollisionSystem::GetInstance().AddLayerTag("Player");
	ge::CollisionSystem::GetInstance().AddLayerTag("Wall");
	ge::CollisionSystem::GetInstance().AddLayerTag("Enemy");

	// -----------------------------------------------
	// Player Initialization
	// -----------------------------------------------
	auto player1GO = std::make_unique<ge::GameObject>("GO_Player1");
	auto player1Image{ player1GO->AddComponent<ge::Image>(player1GO.get()) };
	player1Image->SetTexture(playerTexture);
	auto player1Tr{ player1GO->GetComponent<ge::Transform>() }; 
	player1Tr->SetLocalPosition({ 250.f, 350.f, 0.f });
	player1Tr->SetLocalScale({ 2.5f, 2.5f, 2.5f });

	auto player1BoxColl{ player1GO->AddComponent<ge::BoxCollider>(player1GO.get(), 
		player1Image->GetTexture()->GetSize()) };
	player1BoxColl->AssignTag("Player");

	player1GO->AddComponent<ge::HealthComponent>(player1GO.get(), 3);
	player1GO->AddComponent<ge::ScoreComponent>(player1GO.get(), 0);
	auto player1PlayerComp{ player1GO->AddComponent<PlayerComponent>(player1GO.get(), 120.f) };
	player1PlayerComp->GetDamageEvent().AddObserver(&BombermanSoundManager);
	player1PlayerComp->GetDeadEvent().AddObserver(&BombermanSoundManager);
	player1PlayerComp->GetScoreChangeEvent().AddObserver(&BombermanSoundManager);
	player1GO->AddComponent<BombLayerComponent>(player1GO.get(), bombTexture,
		[]() -> float { return CurrentBombExplosion; }, 1)->GetLaidBombEvent().AddObserver(&BombermanSoundManager);
	

	auto player2GO = std::make_unique<ge::GameObject>("GO_Player2");
	auto player2Image{ player2GO->AddComponent<ge::Image>(player2GO.get()) };
	player2Image->SetTexture(balloonTexture);
	auto player2Tr{ player2GO->GetComponent<ge::Transform>() };
	player2Tr->SetLocalPosition({ 200.f, 150.f, 0.f });
	player2Tr->SetLocalScale({ 2.f, 2.f, 2.f });

	auto player2BoxColl{ player2GO->AddComponent<ge::BoxCollider>(player2GO.get(), 
		player2Image->GetTexture()->GetSize()) };
	player2BoxColl->AssignTag("Player");

	player2GO->AddComponent<ge::HealthComponent>(player2GO.get(), 3);
	player2GO->AddComponent<ge::ScoreComponent>(player2GO.get(), 0);
	auto player2PlayerComp{ player2GO->AddComponent<PlayerComponent>(player2GO.get(), 240.f) };
	player2PlayerComp->GetDamageEvent().AddObserver(&BombermanSoundManager);
	player2PlayerComp->GetDeadEvent().AddObserver(&BombermanSoundManager);
	player2PlayerComp->GetScoreChangeEvent().AddObserver(&BombermanSoundManager);
	player2GO->AddComponent<BombLayerComponent>(player2GO.get(), bombTexture,
		[]() -> float { return CurrentBombExplosion; }, 2)->GetLaidBombEvent().AddObserver(&BombermanSoundManager);

	// -----------------------------------------------
	// Enemy Initialization
	// -----------------------------------------------
	auto enemy1GO = std::make_unique<ge::GameObject>("GO_IceEnemy1");
	auto enemy1Image{ enemy1GO->AddComponent<ge::Image>(enemy1GO.get()) };
	enemy1Image->SetTexture(iceEnemyTexture);
	auto enemy1Tr{ enemy1GO->GetComponent<ge::Transform>() };
	enemy1Tr->SetLocalPosition({ 500.f, 250.f, 0.f });
	enemy1Tr->SetLocalScale({ 2.5f, 2.5f, 2.5f });

	auto enemy1BoxColl{ enemy1GO->AddComponent<ge::BoxCollider>(enemy1GO.get(), 
		enemy1Image->GetTexture()->GetSize()) };
	enemy1BoxColl->AssignTag("Enemy");

	enemy1GO->AddComponent<EnemyComponent>(enemy1GO.get(), 60.f)->AddTarget(player1GO.get());
	enemy1GO->GetComponent<EnemyComponent>()->AddTarget(player2GO.get());

	auto playerInRangePredicate{ MakePlayerInRangePredicate(enemy1GO.get()) };

	auto* enemyFsm{ enemy1GO->AddComponent<ge::FSMComponent>(enemy1GO.get()) };
	auto* wanderStateRaw{ enemyFsm->AddState(std::make_unique<WanderState>(enemy1GO.get())) };
	auto* chaseStateRaw{ enemyFsm->AddState(std::make_unique<ChaseState>(enemy1GO.get())) };
	enemyFsm->AddTransition(wanderStateRaw, chaseStateRaw, playerInRangePredicate);
	enemyFsm->AddTransition(chaseStateRaw, wanderStateRaw, [playerInRangePredicate]()->bool {return !playerInRangePredicate(); });

	enemyFsm->Start();

	// -----------------------------------------------
	// Health Displays
	// -----------------------------------------------
	auto p1HealthDisplayGO = std::make_unique<ge::GameObject>("GO_P1HealthDisplay");
	p1HealthDisplayGO->AddComponent<ge::TextComponent>(p1HealthDisplayGO.get(), "", font, colorBlue);
	p1HealthDisplayGO->AddComponent<HealthDisplayComponent>(p1HealthDisplayGO.get(), player1GO.get());
	p1HealthDisplayGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.05f, windowSize.second * 0.9f, 0.f });

	auto p2HealthDisplayGO = std::make_unique<ge::GameObject>("GO_P2HealthDisplay");
	p2HealthDisplayGO->AddComponent<ge::TextComponent>(p2HealthDisplayGO.get(), "", font, colorRed);
	p2HealthDisplayGO->AddComponent<HealthDisplayComponent>(p2HealthDisplayGO.get(), player2GO.get());
	p2HealthDisplayGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.65f, windowSize.second * 0.9f, 0.f });

	// -----------------------------------------------
	// Score Displays
	// -----------------------------------------------
	auto p1ScoreDisplayGO = std::make_unique<ge::GameObject>("GO_FirstPlayerScore");
	p1ScoreDisplayGO->AddComponent<ge::TextComponent>(p1ScoreDisplayGO.get(), "Score: 0", font, colorBlue);
	p1ScoreDisplayGO->AddComponent<ScoreDisplayComponent>(p1ScoreDisplayGO.get(), player1GO.get());
	p1ScoreDisplayGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.05f, windowSize.second * 0.8f, 0.f });

	auto p2ScoreDisplayGO = std::make_unique<ge::GameObject>("GO_SecondPlayerScore");
	p2ScoreDisplayGO->AddComponent<ge::TextComponent>(p2ScoreDisplayGO.get(), "Score: 0", font, colorRed);
	p2ScoreDisplayGO->AddComponent<ScoreDisplayComponent>(p2ScoreDisplayGO.get(), player2GO.get());
	p2ScoreDisplayGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.65f, windowSize.second * 0.8f, 0.f });

	// -----------------------------------------------
	// Command Binding to two players
	// -----------------------------------------------
#pragma region CommandBinding
	auto& input{ ge::ServiceLocator::GetInputManager() };

	auto* p1GORaw{ player1GO.get() };
	auto* p2GORaw{ player2GO.get() };

	// Player speeds
	auto deathConditionLambda1{ [p1GORaw]() -> bool {
		auto* pc{p1GORaw->GetComponent<PlayerComponent>()};
		return pc && pc->IsAlive(); } };
	auto deathConditionLambda2{ [p2GORaw]() -> bool { 
		auto* pc{p2GORaw->GetComponent<PlayerComponent>()};
		return pc && pc->IsAlive(); } };

	const float firstPlayerSpeed{ p1GORaw->GetComponent<PlayerComponent>()->GetSpeed()};
	const float secondPlayerSpeed{ p2GORaw->GetComponent<PlayerComponent>()->GetSpeed() };

	auto getPlayerSpeedLambda1{ [p1GORaw, firstPlayerSpeed]() -> float 
		{
			auto* pc{ p1GORaw->GetComponent<PlayerComponent>() };
			return pc ? pc->GetSpeed() : firstPlayerSpeed;
		} };

	auto getPlayerSpeedLambda2{ [p2GORaw, secondPlayerSpeed]() -> float
		{
			auto* pc{ p2GORaw->GetComponent<PlayerComponent>() };
			return pc ? pc->GetSpeed() : secondPlayerSpeed;
		} };

	// --------------------
	// First player 
	// --------------------
	input.BindKeyboardCommand(SDL_SCANCODE_SPACE, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<LayBombCommand>(player1GO.get()),
			deathConditionLambda1));
	// Movement
	input.BindKeyboardCommand(SDL_SCANCODE_W, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player1GO.get(),
			glm::vec3{ 0.f, -1.f, 0.f }, getPlayerSpeedLambda1),
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_A, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player1GO.get(),
			glm::vec3{ -1.f, 0.f, 0.f }, getPlayerSpeedLambda1),
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_S, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player1GO.get(),
			glm::vec3{ 0.f, 1.f, 0.f }, getPlayerSpeedLambda1),
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_D, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player1GO.get(),
			glm::vec3{ 1.f, 0.f, 0.f }, getPlayerSpeedLambda1),
			deathConditionLambda1));
	// Damage and score
	input.BindKeyboardCommand(SDL_SCANCODE_X, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<DamageCommand>(player2GO.get(), 1),
			deathConditionLambda1));
	input.BindKeyboardCommand(SDL_SCANCODE_E, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ScoreCommand>(player1GO.get(), 10),
			deathConditionLambda1));

	// ---------------------
	// Second player
	// ---------------------
	input.BindControllerCommand(ge::ControllerButton::B, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<LayBombCommand>(player2GO.get()),
			deathConditionLambda2));
	// Movement
	input.BindControllerCommand(ge::ControllerButton::DpadUp, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player2GO.get(),
			glm::vec3{ 0.f, -1.f, 0.f }, getPlayerSpeedLambda2),
			deathConditionLambda2));
	input.BindControllerCommand(ge::ControllerButton::DpadLeft, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player2GO.get(),
			glm::vec3{ -1.f, 0.f, 0.f }, getPlayerSpeedLambda2),
			deathConditionLambda2));
	input.BindControllerCommand(ge::ControllerButton::DpadDown, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player2GO.get(),
			glm::vec3{ 0.f, 1.f, 0.f }, getPlayerSpeedLambda2),
			deathConditionLambda2));
	input.BindControllerCommand(ge::ControllerButton::DpadRight, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ge::MoveCommand>(player2GO.get(),
			glm::vec3{ 1.f, 0.f, 0.f }, getPlayerSpeedLambda2),
			deathConditionLambda2));
	// Damage and score
	input.BindControllerCommand(ge::ControllerButton::X, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<DamageCommand>(player1GO.get(), 1),
			deathConditionLambda2));
	input.BindControllerCommand(ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ConditionalCommand>(std::make_unique<ScoreCommand>(player2GO.get(), 10),
			deathConditionLambda2));
#pragma endregion

	MainGameplayScene.Add(std::move(player1GO));
	MainGameplayScene.Add(std::move(player2GO));

	MainGameplayScene.Add(std::move(enemy1GO));

	MainGameplayScene.Add(std::move(p1HealthDisplayGO));
	MainGameplayScene.Add(std::move(p2HealthDisplayGO));

	MainGameplayScene.Add(std::move(p1ScoreDisplayGO));
	MainGameplayScene.Add(std::move(p2ScoreDisplayGO));
}
