#include "GameplayState.h"

#include "SceneManager.h"
#include "Scene.h"
#include "BombermanGame.h"
#include "Services/ServiceLocator.h"
#include "Services/SoundSystem.h" // for sound channels enum class
#include "Renderer.h"
#include "ResourceManager.h"

#include "Commands/MoveCommand.h"
#include "Commands/LayBombCommand.h"
#include "Commands/SwitchToTransitionStateCommand.h"
#include "Commands/SkipGameplayStageCommand.h"
#include "Commands/RemoteBombDetonateCommand.h"
#include "Commands/ChangeWindowSizeCommand.h"
#include "Commands/ToggleMuteCommand.h"

#include "GameObject.h"
#include "Components/Image.h"
#include "Components/Transform.h"
#include "Components/TextComponent.h"
#include "Components/FPSComponent.h"
#include "Components/ScoreComponent.h" // for score tracking and saving
#include "Components/TimerDisplayComponent.h"

#include "Components/CameraPlayerFollowComponent.h"
#include "Components/EnemyComponent.h" // only to subscibe to dead event
#include "Components/PlayerComponent.h"
#include "Components/ScoreDisplayComponent.h"

#include "SpawnUtils.h"
#include "SoundManager.h"
#include "LevelLoader.h"
#include "StageLoader.h"
#include "LevelGrid.h"
#include "LevelBuilder.h"
#include "EnemyArchetypes.h"
#include "EnemyType.h" // for spawning enemies when timer is over and enemy controllerTarget
#include "PowerupArchetypes.h"
#include "GameEvents.h"
#include "SoundIds.h"
#include "EngineEvents.h" // for TIMER_REACHED_GOAL event
#include "SpriteSheet.h"
#include "PowerupEffects.h" // to Apply powerups in Notify

#include "VictoryState.h"
#include "LossState.h"
#include "StageTransitionState.h"

#include <utility>
#include <memory>
#include <array>
#include <algorithm>
#include <string>

bombGame::GameplayGameState::GameplayGameState(BombermanGame& game)
	:GameState::GameState(game)
{
}

void bombGame::GameplayGameState::OnEnter()
{
	// =================================================
	// Load Resources
	// =================================================
	const auto backgroundTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_PlayField.png") };
	const auto topGrayTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_GrayTopBar.png") };
	
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

	const auto displaysFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 35) };
	displaysFont->SetBold(true);

	constexpr SDL_Color colorWhite{ SDL_Color{255, 255, 255, 255} };

	// =================================================
	// Scene Initialization:
	// =================================================
	
	// --- Static objects initialization ---
	ge::Scene& gameplayScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::Gameplay) };

	const auto designSize{ ge::Renderer::GetInstance().GetWindowDesignSize() };

	const glm::vec3 topBgPosition{ 0.f, designSize.second / 10.7f, 0.f };
	auto backgroundGO = std::make_unique<ge::GameObject>("GO_Background");
	backgroundGO->AddComponent<ge::Image>(backgroundGO.get())->SetTexture(backgroundTexture);
	backgroundGO->GetComponent<ge::Transform>()->SetLocalScale(3.5f, 3.5f, 1.f);
	backgroundGO->GetComponent<ge::Transform>()->SetLocalPosition(topBgPosition);
	gameplayScene.Add(std::move(backgroundGO));

	auto topBarGO = std::make_unique<ge::GameObject>("GO_GrayTopBar");
	topBarGO->AddComponent<ge::Image>(topBarGO.get())->SetTexture(topGrayTexture);
	topBarGO->GetComponent<ge::Transform>()->SetLocalScale(3.5f, 2.f, 1.f);
	gameplayScene.Add(std::move(topBarGO));

	// =================================================
	// Level Genertion
	// =================================================
	const stageLoader::StageDescriptor& stage{ stageLoader::Load(GetCachedGameSession().currentStageIndex)};

	levelLoader::LevelLayout layout{ levelLoader::Load(
		ge::ResourceManager::GetInstance().GetFullPath("levels/" + stage.layoutFile)) };
	const float tileSize{ (static_cast<float>(designSize.second) - topBgPosition.y) / layout.height };
	const float mapWidth{ tileSize * layout.width };
	const float mapHeight{ tileSize * (layout.height - 1.5f) };

	m_LevelGrid = std::make_unique<LevelGrid>(layout, topBgPosition, tileSize);

	powerupArchetypes::InitializeArchetypes(powerupFireUpTexture, powerupBombUpTexture, powerupRemoteDetonateTexture);

	// Generate Static Wall box colliders
	levelBuilder::BuildStaticGeometry(gameplayScene, *m_LevelGrid.get());
	// Generate Exit, Powerup, DYNAMIC WALLS
	levelBuilder::GenerateDynamicObjects(gameplayScene, *m_LevelGrid.get(), 
		breakableWallSheet, exitDoorTexture, 
		stage.powerupEntry, stage.breakableDensity);

	// -----------------------------------------------
	// Players
	// -----------------------------------------------
	SoundManager& bombermanSoundManager{ GetBombermanGame().GetSoundManager() };
	std::array<ge::SpriteSheet*, 3> explosions{ explosionCenterSpriteSheet, explosionVertSpriteSheet, explosionHorSpriteSheet };
	const int stageStartScore{ GetCachedGameSession().totalScore };

	switch (GetCachedGameSession().currentPlayerMode)
	{
	case PlayerMode::SinglePlayer:
	{
		ge::GameObject* player{ spawnUtils::SpawnPlayerAt(gameplayScene, *m_LevelGrid,
			layout.player1SpawnPoint, playerSpriteSheet,
			&bombermanSoundManager, this,
			bombSpriteSheet, explosions,
			stageStartScore) };
		// !!
		m_TrackedPlayers.push_back(player);
	}
		break;
	case PlayerMode::Coop:
	{
		ge::GameObject* player1{ spawnUtils::SpawnPlayerAt(gameplayScene, *m_LevelGrid,
			layout.player1SpawnPoint, playerSpriteSheet,
			&bombermanSoundManager, this,
			bombSpriteSheet, explosions,
			stageStartScore) };
		// !!
		m_TrackedPlayers.push_back(player1);

		ge::GameObject* player2{ spawnUtils::SpawnPlayerAt(gameplayScene, *m_LevelGrid,
			layout.player2SpawnPoint, playerSpriteSheet,
			&bombermanSoundManager, this,
			bombSpriteSheet, explosions,
			stageStartScore) };
		// !!
		m_TrackedPlayers.push_back(player2);
	}
		break;
	case PlayerMode::Versus:
	{
		ge::GameObject* player{ spawnUtils::SpawnPlayerAt(gameplayScene, *m_LevelGrid,
			layout.player1SpawnPoint, playerSpriteSheet,
			&bombermanSoundManager, this,
			bombSpriteSheet, explosions,
			stageStartScore) };
		// !!
		m_TrackedPlayers.push_back(player);

		ge::GameObject* enemyPlayer{ spawnUtils::SpawnEnemyPlayerAt(gameplayScene, *m_LevelGrid,
			layout.player2SpawnPoint, balloomSpriteSheet,
			&bombermanSoundManager, this) };
		m_TrackedEnemyPlayer = enemyPlayer;
	}
		break;
	}

	// ---------
	// Apply previous powerups to controllerTarget
	// ---------
	for (auto player : m_TrackedPlayers)
	{
		powerupEffects::ApplyStoredToPlayer(player, GetCachedGameSession().storedPowerups);
	}

	// -------------------------------------------------
	// Enemy Generation
	// -------------------------------------------------
	enemyArchetypes::InitializeArchetypes(balloomSpriteSheet, onilEnemySheet, dahlSpriteSheet, minvoSpriteSheet);
	auto spawnedEnemies{ levelBuilder::GenerateEnemies(gameplayScene, *m_LevelGrid,
		stage.enemies,
		m_TrackedPlayers) };

	// Listen to ENEMY_DIED event so we can check if stage is cleared
	for (auto* enemy : spawnedEnemies)
	{
		if (auto* enemyComp = enemy->GetComponent<EnemyComponent>())
		{
			enemyComp->GetDeadEvent().AddObserver(this);
		}
	}

	// !!!
	m_RemainingEnemyCount += static_cast<int>(spawnedEnemies.size());
	if (m_RemainingEnemyCount > 0)
		m_StageCleared = false;

	// -------------------------------------------------
	// Dynamic Text Objects
	// -------------------------------------------------
	// ---- Lives Display - Not the same as HealthDisplay per controllerTarget ----
	auto healthDisplayGO = std::make_unique<ge::GameObject>("GO_SharedLivesDisplay");
	healthDisplayGO->AddComponent<ge::TextComponent>(healthDisplayGO.get(), 
		"LEFT " + std::to_string(GetCachedGameSession().playerLives), displaysFont, colorWhite);
	//healthDisplayGO->AddComponent<HealthDisplayComponent>(healthDisplayGO.get(), player1GO.get());
	healthDisplayGO->GetComponent<ge::Transform>()->SetLocalPosition({
		designSize.first * 0.8f, designSize.second * 0.02f, 0.f });
	healthDisplayGO->SetIgnoreCamera(true);
	gameplayScene.Add(std::move(healthDisplayGO));

	// ---- Score Displays - track controllerTarget[0] since it's always shared ----
	auto scoreDisplayGO = std::make_unique<ge::GameObject>("GO_SharedScoreDisplay");
	scoreDisplayGO->AddComponent<ge::TextComponent>(scoreDisplayGO.get(), "00", displaysFont, colorWhite);
	scoreDisplayGO->AddComponent<ScoreDisplayComponent>(scoreDisplayGO.get(), m_TrackedPlayers[0]);
	scoreDisplayGO->GetComponent<ge::Transform>()->SetLocalPosition({
		designSize.first * 0.5f, designSize.second * 0.02f, 0.f });
	scoreDisplayGO->SetIgnoreCamera(true);
	gameplayScene.Add(std::move(scoreDisplayGO));

	// --- Timer ---
	auto timerGO = std::make_unique<ge::GameObject>("GO_Timer");
	timerGO->AddComponent<ge::TextComponent>(timerGO.get(), "", displaysFont, colorWhite);
	timerGO->GetComponent<ge::Transform>()->SetLocalPosition({ 10.f, 10.f, 0.f });
	auto* timerComp{ timerGO->AddComponent<ge::TimerDisplayComponent>(timerGO.get(), GameplayTime,
		0.f, ge::TimerDisplayComponent::TimerType::Decrementing, 1.f) };
	timerGO->SetIgnoreCamera(true);
	timerComp->GetReachedGoalEvent().AddObserver(this);
	m_TrackedTimer = timerGO.get();
	gameplayScene.Add(std::move(timerGO));

	// -- FPS Debug Text ---
#if _DEBUG
	constexpr SDL_Color colorRed{ SDL_Color{255, 0, 0, 255} };
	const auto debugFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 30) };
	debugFont->SetBold(true);

	auto fpsGO = std::make_unique<ge::GameObject>("GO_FPSTextObject");
	fpsGO->AddComponent<ge::TextComponent>(fpsGO.get(), "", debugFont, colorRed);
	fpsGO->AddComponent<ge::FPSComponent>(fpsGO.get());
	fpsGO->GetComponent<ge::Transform>()->
		SetLocalPosition({ 10.f, designSize.second * 0.9f, 0.f });
	fpsGO->SetIgnoreCamera(true);

	gameplayScene.Add(std::move(fpsGO));
#endif

	// =================================================
	// Camera + Folow
	// =================================================
	m_GameplayCamera = std::make_unique<ge::Camera>(glm::vec2{
		static_cast<float>(ge::Renderer::GetInstance().GetWindowDesignSize().first),
		static_cast<float>(ge::Renderer::GetInstance().GetWindowDesignSize().second) });
	m_GameplayCamera->SetBounds(
		{ topBgPosition.x, topBgPosition.y },
		{ topBgPosition.x + mapWidth, topBgPosition.y + mapHeight }
	);

	ge::Renderer::GetInstance().SetActiveCamera(m_GameplayCamera.get());

	auto cameraGO = std::make_unique<ge::GameObject>("GO_Camera");
	auto* followCam{ cameraGO->AddComponent<CameraPlayerFollowComponent>(
		cameraGO.get(), m_GameplayCamera.get(), 3.f) };
	for (auto* player : m_TrackedPlayers)
	{
		followCam->AddTarget(player);
	}
	if (m_TrackedEnemyPlayer)
		followCam->AddTarget(m_TrackedEnemyPlayer);
	gameplayScene.Add(std::move(cameraGO));

	// =================================================
	// Specify Game State Input Bindings
	// =================================================
#pragma region CommandBinding
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };

	ge::GameObject* keyboardTarget{ nullptr };
	ge::GameObject* controllerTarget0{ nullptr };
	ge::GameObject* controllerTarget1{ nullptr };

	const int numControllers{ inputManager.GetControllerCount() };
	switch (GetCachedGameSession().currentPlayerMode)
	{
	case PlayerMode::SinglePlayer:
		keyboardTarget = m_TrackedPlayers[0];
		controllerTarget0 = m_TrackedPlayers[0];
		break;
	case PlayerMode::Coop:
		if (numControllers >= 2)
		{
			controllerTarget0 = m_TrackedPlayers[0];
			controllerTarget1 = m_TrackedPlayers[1];
		}
		else
		{
			keyboardTarget = m_TrackedPlayers[0];
			controllerTarget0 = m_TrackedPlayers[1];
		}
		break;
	case PlayerMode::Versus:
		if (numControllers >= 2)
		{
			controllerTarget0 = m_TrackedPlayers[0];
			controllerTarget1 = m_TrackedEnemyPlayer;
		}
		else
		{
			keyboardTarget = m_TrackedPlayers[0];
			controllerTarget0 = m_TrackedEnemyPlayer;
		}
		break;
	}

	inputManager.BindKeyboardCommand(SDL_SCANCODE_R, ge::InputManager::InputTrigger::Up,
		std::make_unique<SwitchToTransitionCommand>(GetBombermanGame()));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_F1, ge::InputManager::InputTrigger::Up,
		std::make_unique<SkipGameplayStageCommand>(GetBombermanGame()));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_F2, ge::InputManager::InputTrigger::Up,
		std::make_unique<ToggleMuteCommand>(GetBombermanGame()));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_F11, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ChangeWindowSizeCommand>(1200, 1200));

	// --------------------
	// Keyboard
	// --------------------
	if (keyboardTarget) 
		BindKeyboardForPlayer(keyboardTarget); 

	// ---------------------
	// Controller
	// ---------------------
	if (controllerTarget0)
		BindControllerForPlayer(0, controllerTarget0);
	if (controllerTarget1) 
		BindControllerForPlayer(1, controllerTarget1);
#pragma endregion

	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::GameplayNormalOST, 0.2f, ge::SoundCategory::Music);
	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::Gameplay);
}

void bombGame::GameplayGameState::OnExit()
{
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };
	inputManager.UnbindAllController();
	inputManager.UnbindAllKeyboard();

	// ~Camera already does this, this is defensive
	ge::Renderer::GetInstance().SetActiveCamera(nullptr); // clear the camera

	GetBombermanGame().GetSoundManager().StopAllSounds();
	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::Gameplay);

	// Just to be safe -> clear tracked data
	m_TrackedPlayers.clear();
	m_TrackedTimer = nullptr;
	m_TrackedEnemyPlayer = nullptr;
	m_RemainingEnemyCount = 0;
	m_TrackedPickedPowerupsStage.clear();
}

std::unique_ptr<bombGame::GameState> bombGame::GameplayGameState::Update(float)
{
	// Reset GameplayNormalOST State + stage if all(1 or 2) players are dead
	if (std::all_of(m_TrackedPlayers.begin(), m_TrackedPlayers.end(), [](ge::GameObject* player)->bool
		{
			auto* playerComp{ player->GetComponent<PlayerComponent>() };
			if (playerComp && !playerComp->IsAlive())
				return true;

			return false;
		}))
	{
		// Check Defeat - go into Loss State, else go into StageTransitionState -> GameplayState
		if (GetCachedGameSession().playerLives <= 1)
		{
			GetBombermanGame().FailStage(m_TrackedPlayers[0]->GetComponent<ge::ScoreComponent>()->GetCurrentScore());
			return std::make_unique<LossState>(GetBombermanGame());
		}
		else
		{
			GetBombermanGame().FailStage();
			return std::make_unique<StageTransitionState>(GetBombermanGame());
		}
	}
	else
	{
		// If there are still alive enemies
		if (!m_StageCleared)
		{
			// Else, stay in this state
			return nullptr;
		}

		// If stage is clear but no controllerTarget is on the exit, stay in this state
		if (!IsAnyPlayerOnExit())
			return nullptr;

		// If stage is clear AND controllerTarget is on exit -> Progress gameplay + reset gameplay or go to victory
		GetBombermanGame().CompleteStage(m_TrackedPlayers[0]->GetComponent<ge::ScoreComponent>()->GetCurrentScore(),
			m_TrackedPickedPowerupsStage);

		if (GetCachedGameSession().currentStageIndex >= stageLoader::GetStageCount())
		{
			return std::make_unique<VictoryState>(GetBombermanGame());
		}
		else
		{
			return std::make_unique<StageTransitionState>(GetBombermanGame());
		}
	}
}

void bombGame::GameplayGameState::Notify(int eventId, ge::GameObject* sourceObj)
{
	switch (eventId)
	{
	case events::GameEventId::ENEMY_DIED:
	{
		// --- Stage Cleared logic ---
		--m_RemainingEnemyCount;
		if (m_RemainingEnemyCount == 0)
		{
			m_StageCleared = true;
		}

		// --- Awarding Player score logic ---
		if (!sourceObj)
			return;

		const auto enemyComp{ sourceObj->GetComponent<EnemyComponent>() };
		if (!enemyComp)
			return;

		const int scorePerEnemy{ enemyComp->GetScoreValue() };
		for (auto* player : m_TrackedPlayers)
		{
			if (!player)
				continue;

			auto* playerComp{ player->GetComponent<PlayerComponent>() };
			if (!playerComp || !playerComp->IsAlive())
				continue;

			if (auto* scoreComp = player->GetComponent<ge::ScoreComponent>())
			{
				scoreComp->ChangeScore(scorePerEnemy);
			}
		}
	}
		break;
	case events::GameEventId::POWERUP_PICKED_UP:
	{
		// Apply a powerup to ALL (alive) tracked players
		const auto powerupType{ sourceObj->GetComponent<PlayerComponent>()->GetLastPickedPowerupType() };
		powerupEffects::OnPickup(powerupType, m_TrackedPlayers);
		m_TrackedPickedPowerupsStage.push_back(powerupType);
	}
		break;
	case ge::events::EngineEventId::TIMER_REACHED_GOAL:
		if (sourceObj == m_TrackedTimer)
		{
			auto spawnedEnemies{ spawnUtils::SpawnEnemiesAtExit(*ge::SceneManager::GetInstance().GetCurrentActiveScene(),
			m_LevelGrid.get(), { {EnemyType::Dall, 5} }, m_TrackedPlayers) };

			// Listen to ENEMY_DIED event so we can check if stage is cleared
			for (auto* enemy : spawnedEnemies)
			{
				if (auto* enemyComp = enemy->GetComponent<EnemyComponent>())
				{
					enemyComp->GetDeadEvent().AddObserver(this);
				}
			}

			// !!!
			m_RemainingEnemyCount += static_cast<int>(spawnedEnemies.size());
		}
		break;
	default:
		break;
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

void bombGame::GameplayGameState::BindKeyboardForPlayer(ge::GameObject* keyboardTarget)
{
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };

	inputManager.BindKeyboardCommand(SDL_SCANCODE_SPACE, ge::InputManager::InputTrigger::Up,
		std::make_unique<LayBombCommand>(keyboardTarget));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_X, ge::InputManager::InputTrigger::Up,
		std::make_unique<RemoteBombDetonateCommand>(keyboardTarget));
	// Movement
	inputManager.BindKeyboardCommand(SDL_SCANCODE_W, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(keyboardTarget, glm::vec3{ 0.f, -1.f, 0.f }));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_A, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(keyboardTarget, glm::vec3{ -1.f, 0.f, 0.f }));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_S, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(keyboardTarget, glm::vec3{ 0.f, 1.f, 0.f }));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_D, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(keyboardTarget, glm::vec3{ 1.f, 0.f, 0.f }));
}

void bombGame::GameplayGameState::BindControllerForPlayer(unsigned int controllerIdx, ge::GameObject* controllerTarget)
{
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };

	inputManager.BindControllerCommand(controllerIdx, ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
		std::make_unique<LayBombCommand>(controllerTarget));
	inputManager.BindControllerCommand(controllerIdx, ge::ControllerButton::B, ge::InputManager::InputTrigger::Up,
		std::make_unique<RemoteBombDetonateCommand>(controllerTarget));
	// Movement
	inputManager.BindControllerCommand(controllerIdx, ge::ControllerButton::DpadUp, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(controllerTarget, glm::vec3{ 0.f, -1.f, 0.f }));
	inputManager.BindControllerCommand(controllerIdx, ge::ControllerButton::DpadLeft, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(controllerTarget, glm::vec3{ -1.f, 0.f, 0.f }));
	inputManager.BindControllerCommand(controllerIdx, ge::ControllerButton::DpadDown, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(controllerTarget, glm::vec3{ 0.f, 1.f, 0.f }));
	inputManager.BindControllerCommand(controllerIdx, ge::ControllerButton::DpadRight, ge::InputManager::InputTrigger::Pressed,
		std::make_unique<MoveCommand>(controllerTarget, glm::vec3{ 1.f, 0.f, 0.f }));
}
