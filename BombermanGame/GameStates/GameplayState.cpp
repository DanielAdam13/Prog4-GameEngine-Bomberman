#include "GameplayState.h"

#include "SceneManager.h"
#include "Scene.h"
#include "BombermanGame.h"
#include "Services/ServiceLocator.h"
#include "Renderer.h"
#include "ResourceManager.h"

#include "Commands/ConditionalCommand.h"
#include "Commands/ChangeWindowSizeCommand.h"

#include "Commands/MoveCommand.h"
#include "Commands/DamageCommand.h"
#include "Commands/ScoreCommand.h"
#include "Commands/LayBombCommand.h"
#include "Commands/SwitchToGameplayStateCommand.h"
#include "Commands/SkipGameplayStageCommand.h"
#include "Commands/RemoteBombDetonateCommand.h"

#include "GameObject.h"
#include "Components/Image.h"
#include "Components/Transform.h"
#include "Components/TextComponent.h"
#include "Components/FPSComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/AnimatorComponent.h"
#include "Components/Colliders.h"
#include "Components/CameraPlayerFollowComponent.h"
#include "Components/EnemyComponent.h" // only to subscibe to dead event

#include "Components/PlayerComponent.h"
#include "Components/BombLayerComponent.h"
#include "Components/HealthDisplayComponent.h"
#include "Components/ScoreDisplayComponent.h"

#include "Animation.h"
#include "SpawnUtils.h"
#include "SoundManager.h"
#include "LevelLoader.h"
#include "StageLoader.h"
#include "LevelGrid.h"
#include "LevelBuilder.h"
#include "EnemyArchetypes.h"
#include "PowerupArchetypes.h"
#include "GameEvents.h"

#include "VictoryState.h"

#include <utility>
#include <memory>
#include <array>

bombGame::GameplayGameState::GameplayGameState(BombermanGame& game)
	:GameState::GameState(game)
{
}

void bombGame::GameplayGameState::OnEnter()
{
	SoundManager& bombermanSoundManager{ GetBombermanGame().GetSoundManager() };
	const auto rendWindowDesignSize{ ge::Renderer::GetInstance().GetWindowDesignSize() };

	// =================================================
	// Load Resources
	// =================================================
	const auto font{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 28) };
	font->SetBold(true);

	//constexpr SDL_Color colorBlack{ SDL_Color{0, 0, 0, 255} };
	constexpr SDL_Color colorRed{ SDL_Color{120, 20, 50, 255} };
	constexpr SDL_Color colorBlue{ SDL_Color{45, 50, 130, 255} };
	const auto tutFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 20) };
	tutFont->SetBold(true);

	const auto backgroundTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_PlayField.png") };
	
	const auto powerupFireUpTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_PowerupFireUp.png") };
	const auto powerupBombUpTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_PowerupBombUp.png") };
	const auto powerupRemoteDetonateTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_PowerupRemoteDetonate.png") };

	const auto playerSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_Player.png", 7, 3) };

	const auto balloomSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_Balloom.png", 11, 1) };
	const auto onilEnemySheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_Onil.png", 11, 1) };
	const auto dahlSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_Dahl.png", 11, 1) };
	const auto minvoSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_Minvo.png", 11, 1) };

	const auto exitDoorTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_DoorExit.png") };
	const auto breakableWallSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_BreakableWall.png", 7, 1) };
	const auto bombSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_Bomb.png", 3, 1) };
	const auto explosionCenterSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_CenterExplosion.png", 4, 1) };
	const auto explosionHorSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_HorExplosion.png", 4, 2) };
	const auto explosionVertSpriteSheet{ ge::ResourceManager::GetInstance().LoadSpriteSheet("sprites/I_SpriteSheet_VertExplosion.png", 4, 2) };

	// =================================================
	// Scene Initialization:
	// =================================================
	
	// --- Static objects in scene initialization ---
	ge::Scene& GameplayScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::Gameplay) };

	const auto windowSize{ ge::Renderer::GetInstance().GetWindowSize() };

	const glm::vec3 topBgPosition{ 0.f, windowSize.second / 10.7f, 0.f };
	auto backgroundGO = std::make_unique<ge::GameObject>("GO_Background");
	backgroundGO->AddComponent<ge::Image>(backgroundGO.get())->SetTexture(backgroundTexture);
	backgroundGO->GetComponent<ge::Transform>()->SetLocalScale(windowSize.first / rendWindowDesignSize.first * 3.5f,
		windowSize.second / rendWindowDesignSize.second * 3.5f, 1.f);

	backgroundGO->GetComponent<ge::Transform>()->SetLocalPosition(topBgPosition);
	GameplayScene.Add(std::move(backgroundGO));
#if _DEBUG
	auto textGO = std::make_unique<ge::GameObject>("GO_TextObject");
	textGO->AddComponent<ge::TextComponent>(textGO.get(), "0.00 FPS", font, colorRed);
	textGO->AddComponent<ge::FPSComponent>(textGO.get());

	GameplayScene.Add(std::move(textGO));
#endif

	auto tutorial1GO = std::make_unique<ge::GameObject>("GO_TutorialText1");
	tutorial1GO->AddComponent<ge::TextComponent>(tutorial1GO.get(),
		"D-Pad to move the Balloon | A bomb | B detonate(if)", tutFont, colorRed);
	auto tutorial2GO = std::make_unique<ge::GameObject>("GO_TutorialText1");
	tutorial2GO->AddComponent<ge::TextComponent>(tutorial2GO.get(),
		"WASD to move the BomberMan | SPACE bomb | X detonate(if) ", tutFont, colorBlue);

	tutorial1GO->GetComponent<ge::Transform>()->SetLocalPosition(glm::vec3{ 0.f, windowSize.second / 10, 0.f });
	tutorial2GO->GetComponent<ge::Transform>()->SetLocalPosition(glm::vec3{ 0.f, windowSize.second / 6, 0.f });

	GameplayScene.Add(std::move(tutorial1GO));
	GameplayScene.Add(std::move(tutorial2GO));

	// =================================================
	// Level Genertion
	// =================================================
	const stageLoader::StageDescriptor& stage{ stageLoader::Load(GetBombermanGame().GetCurrentGameSession().currentStageIndex)};

	levelLoader::LevelLayout layout{ levelLoader::Load(
		ge::ResourceManager::GetInstance().GetFullPath("levels/" + stage.layoutFile)) };
	const float tileSize{ (static_cast<float>(windowSize.second) - topBgPosition.y) / layout.height };

	m_LevelGrid = std::make_unique<LevelGrid>(layout, topBgPosition, tileSize);

	powerupArchetypes::InitializeArchetypes(powerupFireUpTexture, powerupBombUpTexture, powerupRemoteDetonateTexture);

	// Generate Static Wall box colliders
	levelBuilder::BuildStaticGeometry(GameplayScene, *m_LevelGrid.get());
	// Generate Exit, Powerup, DYNAMIC WALLS
	levelBuilder::GenerateDynamicObjects(GameplayScene, *m_LevelGrid.get(), 
		breakableWallSheet, exitDoorTexture, 
		stage.powerupType, stage.breakableDensity);

	// -----------------------------------------------
	// Players
	// -----------------------------------------------
	// Player 1
	auto player1GO = std::make_unique<ge::GameObject>("GO_Player1");

	auto player1Animator{ player1GO->AddComponent<ge::AnimatorComponent>(player1GO.get(), playerSpriteSheet) };
	player1Animator->AddAnimation({ "idle", {3}, 1, false });
	player1Animator->AddAnimation({ "walk_left", {0, 1, 2}, 10, true });
	player1Animator->AddAnimation({ "walk_down", {3, 4, 5}, 10, true });
	player1Animator->AddAnimation({ "walk_right", {7, 8, 9}, 10, true });
	player1Animator->AddAnimation({ "walk_up", {10, 11, 12}, 10, true });
	player1Animator->AddAnimation({ "death", { 14, 15, 16, 17, 18, 19, 20}, 3, false });

	auto player1Tr{ player1GO->GetComponent<ge::Transform>() };
	const glm::vec3 player1Pos{
		topBgPosition.x + layout.player1SpawnPoint.first * tileSize,
		topBgPosition.y + layout.player1SpawnPoint.second * tileSize, 0.f };
	player1Tr->SetLocalPosition(player1Pos);
	const float playerScale{ m_LevelGrid->GetTileSize() / (playerSpriteSheet->GetFrameWidth() + 1) };
	player1Tr->SetLocalScale({ playerScale, playerScale, 1.f });

	auto player1BoxColl{ player1GO->AddComponent<ge::BoxCollider>(player1GO.get(),
		player1Animator->GetSingleFrameRectSize()) };
	player1BoxColl->AssignTag("Player");

	player1GO->AddComponent<ge::HealthComponent>(player1GO.get(), 1);
	player1GO->AddComponent<ge::ScoreComponent>(player1GO.get(), 0);
	auto player1PlayerComp{ player1GO->AddComponent<PlayerComponent>(player1GO.get(), 150.f) };
	player1PlayerComp->GetDamageEvent().AddObserver(&bombermanSoundManager);
	player1PlayerComp->GetDeadEvent().AddObserver(&bombermanSoundManager);
	player1PlayerComp->GetScoreChangeEvent().AddObserver(&bombermanSoundManager);
	player1PlayerComp->GetMovedHorEvent().AddObserver(&bombermanSoundManager);
	player1PlayerComp->GetMovedVertEvent().AddObserver(&bombermanSoundManager);
	player1PlayerComp->GetPowerupPickedUpEvent().AddObserver(&bombermanSoundManager);

	std::array<ge::SpriteSheet*, 3> explosions{ explosionCenterSpriteSheet, explosionVertSpriteSheet, explosionHorSpriteSheet };
	auto player1BombLayer{ player1GO->AddComponent<BombLayerComponent>(player1GO.get(), m_LevelGrid.get(), 
		bombSpriteSheet, explosions,
		3.f, 1, 1) };
	player1BombLayer->GetLaidBombEvent().AddObserver(&bombermanSoundManager);
	player1BombLayer->GetBombExplodedEvent().AddObserver(&bombermanSoundManager);

	//!!
	m_TrackedPlayers.push_back(player1GO.get());

	// Player 2
	auto player2GO = std::make_unique<ge::GameObject>("GO_Player2");
	
	auto player2Animator{ player2GO->AddComponent<ge::AnimatorComponent>(player2GO.get(), balloomSpriteSheet) };
	player2Animator->AddAnimation({ "idle", {0}, 1, false });
	player2Animator->AddAnimation({ "walk_left", {3, 4, 5}, 4, true });
	player2Animator->AddAnimation({ "walk_down", {3, 4, 5}, 4, true });
	player2Animator->AddAnimation({ "walk_right", {0, 1, 2}, 4, true });
	player2Animator->AddAnimation({ "walk_up", {0, 1, 2}, 4, true });
	player2Animator->AddAnimation({ "death", { 6, 7, 8, 9, 10 }, 3, false });

	auto player2Tr{ player2GO->GetComponent<ge::Transform>() };
	const glm::vec3 player2Pos{
		topBgPosition.x + layout.player2SpawnPoint.first * tileSize,
		topBgPosition.y + layout.player2SpawnPoint.second * tileSize, 0.f };
	player2Tr->SetLocalPosition(player2Pos);
	player2Tr->SetLocalScale({ 2.f, 2.f, 2.f });

	auto player2BoxColl{ player2GO->AddComponent<ge::BoxCollider>(player2GO.get(),
		player2Animator->GetSingleFrameRectSize()) };
	player2BoxColl->AssignTag("Player");

	player2GO->AddComponent<ge::HealthComponent>(player2GO.get(), 1);
	player2GO->AddComponent<ge::ScoreComponent>(player2GO.get(), 0);
	auto player2PlayerComp{ player2GO->AddComponent<PlayerComponent>(player2GO.get(), 120.f) };
	player2PlayerComp->GetDamageEvent().AddObserver(&bombermanSoundManager);
	player2PlayerComp->GetDeadEvent().AddObserver(&bombermanSoundManager);
	player2PlayerComp->GetScoreChangeEvent().AddObserver(&bombermanSoundManager);
	player2PlayerComp->GetMovedHorEvent().AddObserver(&bombermanSoundManager);
	player2PlayerComp->GetMovedVertEvent().AddObserver(&bombermanSoundManager);
	player2PlayerComp->GetPowerupPickedUpEvent().AddObserver(&bombermanSoundManager);

	auto player2BombLayer{ player2GO->AddComponent<BombLayerComponent>(player2GO.get(), m_LevelGrid.get(), 
		bombSpriteSheet, explosions,
		3.f, 1, 2) };
	player2BombLayer->GetLaidBombEvent().AddObserver(&bombermanSoundManager);
	player2BombLayer->GetBombExplodedEvent().AddObserver(&bombermanSoundManager);

	//!!
	m_TrackedPlayers.push_back(player2GO.get());

	// =================================================
	// Enemy Generation
	// =================================================
	enemyArchetypes::InitializeArchetypes(balloomSpriteSheet, onilEnemySheet, dahlSpriteSheet, minvoSpriteSheet);

	auto spawnedEnemies{ levelBuilder::GenerateEnemies(GameplayScene, *m_LevelGrid,
		stage.enemies,
		{ player1GO.get(), player2GO.get() }) };

	// Listen to ENEMY_DIED event so we can check if stage is cleared
	for (auto* enemy : spawnedEnemies)
	{
		if (auto* enemyComp = enemy->GetComponent<EnemyComponent>())
		{
			enemyComp->GetDeadEvent().AddObserver(this);
		}
	}

	// !!!
	m_RemainingEnemyCount = static_cast<int>(spawnedEnemies.size());
	if (m_RemainingEnemyCount > 0)
		m_StageCleared = false;

	// ---- Health Displays ----
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

	// ---- Score Displays ----
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

	// =================================================
	// Camera + Folow
	// =================================================
	const float mapWidth{ tileSize * layout.width };
	const float mapHeight{ tileSize * (layout.height - 1.5f) };

	m_GameplayCamera = std::make_unique<ge::Camera>(glm::vec2{
		static_cast<float>(ge::Renderer::GetInstance().GetWindowSize().first),
		static_cast<float>(ge::Renderer::GetInstance().GetWindowSize().second) });
	m_GameplayCamera->SetBounds(
		{ topBgPosition.x, topBgPosition.y },
		{ topBgPosition.x + mapWidth, topBgPosition.y + mapHeight }
	);

	ge::Renderer::GetInstance().SetActiveCamera(m_GameplayCamera.get());

	auto cameraGO = std::make_unique<ge::GameObject>("GO_Camera");
	auto* followCam{ cameraGO->AddComponent<CameraPlayerFollowComponent>(
		cameraGO.get(), m_GameplayCamera.get(), 3.f) };
	followCam->AddTarget(player1GO.get());
	followCam->AddTarget(player2GO.get());
	GameplayScene.Add(std::move(cameraGO));

	// =================================================
	// Specify Game State Input Bindings
	// =================================================
#pragma region CommandBinding
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };

	/*inputManager.BindKeyboardCommand(SDL_SCANCODE_F11, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ChangeWindowSizeCommand>(1200, 1200));*/

	inputManager.BindKeyboardCommand(SDL_SCANCODE_R, ge::InputManager::InputTrigger::Up,
		std::make_unique<SwitchToGameplayCommand>(GetBombermanGame()));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_F2, ge::InputManager::InputTrigger::Up,
		std::make_unique<SkipGameplayStageCommand>(GetBombermanGame()));

	// --------------------
	// First player 
	// --------------------
	inputManager.BindKeyboardCommand(SDL_SCANCODE_SPACE, ge::InputManager::InputTrigger::Up,
		std::make_unique<LayBombCommand>(player1GO.get()));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_X, ge::InputManager::InputTrigger::Up,
		std::make_unique<RemoteBombDetonateCommand>(player1GO.get()));
	// Movement
	inputManager.BindKeyboardCommand(SDL_SCANCODE_W, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player1GO.get(), glm::vec3{ 0.f, -1.f, 0.f }));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_A, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player1GO.get(), glm::vec3{ -1.f, 0.f, 0.f }));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_S, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player1GO.get(), glm::vec3{ 0.f, 1.f, 0.f }));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_D, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player1GO.get(), glm::vec3{ 1.f, 0.f, 0.f }));

	// ---------------------
	// Second player
	// ---------------------
	inputManager.BindControllerCommand(ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
		std::make_unique<LayBombCommand>(player2GO.get()));
	inputManager.BindControllerCommand(ge::ControllerButton::B, ge::InputManager::InputTrigger::Up,
		std::make_unique<RemoteBombDetonateCommand>(player2GO.get()));
	// Movement
	inputManager.BindControllerCommand(ge::ControllerButton::DpadUp, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player2GO.get(), glm::vec3{ 0.f, -1.f, 0.f }));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadLeft, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player2GO.get(), glm::vec3{ -1.f, 0.f, 0.f }));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadDown, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player2GO.get(), glm::vec3{ 0.f, 1.f, 0.f }));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadRight, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(player2GO.get(), glm::vec3{ 1.f, 0.f, 0.f }));
#pragma endregion

	GameplayScene.Add(std::move(player1GO));
	GameplayScene.Add(std::move(player2GO));

	GameplayScene.Add(std::move(p1HealthDisplayGO));
	GameplayScene.Add(std::move(p2HealthDisplayGO));

	GameplayScene.Add(std::move(p1ScoreDisplayGO));
	GameplayScene.Add(std::move(p2ScoreDisplayGO));

	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::Gameplay);
}

void bombGame::GameplayGameState::OnExit()
{
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };
	inputManager.UnbindAllController();
	inputManager.UnbindAllKeyboard();

	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::Gameplay);
}

std::unique_ptr<bombGame::GameState> bombGame::GameplayGameState::Update(float)
{
	if (!m_StageCleared)
		return nullptr;

	if (!IsAnyPlayerOnExit())
		return nullptr;

	GetBombermanGame().IncrementGameplayStageIndex();
	if (GetBombermanGame().GetCurrentGameSession().currentStageIndex >= stageLoader::GetStageCount())
	{
		return std::make_unique<VictoryState>(GetBombermanGame());
	}
	else
	{
		return std::make_unique<GameplayGameState>(GetBombermanGame());
	}
}

void bombGame::GameplayGameState::Notify(int eventId, ge::GameObject*)
{
	if (eventId == GameEventId::ENEMY_DIED)
	{
		--m_RemainingEnemyCount;
		if (m_RemainingEnemyCount == 0)
		{
			m_StageCleared = true;
		}
	}
}

bool bombGame::GameplayGameState::IsAnyPlayerOnExit() const noexcept
{
	const auto exitCoord{ m_LevelGrid->GetExitLocation() };
	for (auto* player : m_TrackedPlayers)
	{
		if (!player)
			continue;
		const auto pos{ player->GetComponent<ge::Transform>()->GetWorldPosition() };
		const auto playerTile{ m_LevelGrid->GetGridTileAt(pos) };
		if (playerTile && playerTile->col == exitCoord.first && playerTile->row == exitCoord.second)
		{
			return true;
		}
	}

	return false;
}
