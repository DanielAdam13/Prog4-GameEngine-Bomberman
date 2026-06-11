#include "BombermanGame.h"

#include "Renderer.h"
#include "Services/ServiceLocator.h"
#include "Services/SoundSystem.h"
#include "SoundIds.h"
#include "ResourceManager.h"

#include "Services/InputManager.h"
#include "Commands/ChangeWindowSizeCommand.h"

#include "SoundManager.h"

#include "CollisionSystem.h"

bombGame::BombermanGame::BombermanGame()
	:m_StoredSoundSystem{ &ge::ServiceLocator::GetSoundSystem() }
{
	ge::Renderer::GetInstance().SetWindowSize({ 800, 800 });
}

void bombGame::BombermanGame::Load()
{
	/*ge::ServiceLocator::GetInputManager().BindKeyboardCommand(SDL_SCANCODE_F11, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ChangeWindowSizeCommand>(1200, 1200));*/

	LoadSound();

	// Collision
	ge::CollisionSystem::GetInstance().AddLayerTag("Player");
	ge::CollisionSystem::GetInstance().AddLayerTag("Wall");
	ge::CollisionSystem::GetInstance().AddLayerTag("BreakableWall");
	ge::CollisionSystem::GetInstance().AddLayerTag("Enemy");
	ge::CollisionSystem::GetInstance().AddLayerTag("Explosion");
	ge::CollisionSystem::GetInstance().AddLayerTag("Powerup");

	m_GameStateMachine.SetInitialState(*this);
}

void bombGame::BombermanGame::Update(float deltaTime)
{
	m_GameStateMachine.Update(deltaTime);
}

void bombGame::BombermanGame::FixedUpdate(float fixedDeltaSec)
{
	m_GameStateMachine.FixedUpdate(fixedDeltaSec);
}

bombGame::SoundManager& bombGame::BombermanGame::GetSoundManager() noexcept
{
	return m_BombermanSoundManager;
}

ge::SoundSystem* bombGame::BombermanGame::GetStoredSoundSystem() noexcept
{
	return m_StoredSoundSystem;
}

bombGame::GameStateMachine& bombGame::BombermanGame::GetStateMachine() noexcept
{
	return m_GameStateMachine;
}

const bombGame::GameSession& bombGame::BombermanGame::GetCurrentGameSession() const noexcept
{
	return m_CurrentGameSession;
}

void bombGame::BombermanGame::ClearGameSession()
{
	// Saves only player mode
	m_CurrentGameSession.currentStageIndex = 0;
	m_CurrentGameSession.playerLives = 4;
	m_CurrentGameSession.storedPowerups = {};
	m_CurrentGameSession.totalScore = 0;
}

void bombGame::BombermanGame::FailStage(int lossScore)
{
	--m_CurrentGameSession.playerLives;
	m_CurrentGameSession.totalScore += lossScore;
}

void bombGame::BombermanGame::CompleteStage(int score, std::vector<PowerupType> pickedPowerups)
{
	// Progress stage index
	++m_CurrentGameSession.currentStageIndex;
	// Save score
	m_CurrentGameSession.totalScore += score;
	// Save powerups
	for (auto& powerType : pickedPowerups)
	{
		m_CurrentGameSession.storedPowerups.push_back(powerType);
	}
}

void bombGame::BombermanGame::SetPlayerMode(PlayerMode mode)
{
	m_CurrentGameSession.currentPlayerMode = mode;
}

void bombGame::BombermanGame::LoadSound()
{
	m_StoredSoundSystem->RegisterSound(SoundIds::ExplosionBomb, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomb_explosion.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::BombermanDied, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomberman_killed.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::LayBomb, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomb_lay.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::Pause, ge::ResourceManager::GetInstance().GetFullPath("sounds/pause.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::Powerup, ge::ResourceManager::GetInstance().GetFullPath("sounds/powerup.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::StepHorizontal, ge::ResourceManager::GetInstance().GetFullPath("sounds/step_horizontal.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::StepVertical, ge::ResourceManager::GetInstance().GetFullPath("sounds/step_vertical.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::MainMenuOST, ge::ResourceManager::GetInstance().GetFullPath("sounds/main_menu.mp3"));
	m_StoredSoundSystem->RegisterSound(SoundIds::GameplayNormalOST, ge::ResourceManager::GetInstance().GetFullPath("sounds/gameplay_normal.mp3"));
	m_StoredSoundSystem->RegisterSound(SoundIds::StageEnter, ge::ResourceManager::GetInstance().GetFullPath("sounds/stage_enter.mp3"));
	m_StoredSoundSystem->RegisterSound(SoundIds::StageWon, ge::ResourceManager::GetInstance().GetFullPath("sounds/stage_won.mp3"));
	m_StoredSoundSystem->RegisterSound(SoundIds::StageLost, ge::ResourceManager::GetInstance().GetFullPath("sounds/stage_lost.mp3"));
	m_StoredSoundSystem->RegisterSound(SoundIds::GameLost, ge::ResourceManager::GetInstance().GetFullPath("sounds/game_lost.mp3"));
}
