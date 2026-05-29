#include "BombermanGame.h"

#include "Renderer.h"
#include "Services/ServiceLocator.h"
#include "Services/SoundSystem.h"
#include "SoundIds.h"
#include "ResourceManager.h"

#include "Services/InputManager.h"
#include "Commands/ChangeWindowSizeCommand.h"

#include "SoundManager.h"

#include "GameStates/MainMenuState.h"

#include "CollisionSystem.h"

#include <memory>
#include <utility>

bombGame::BombermanGame::BombermanGame()
{
	m_StoredSoundSystem = &ge::ServiceLocator::GetSoundSystem();
	ge::Renderer::GetInstance().SetWindowSize({ 800, 800 });
}

bombGame::BombermanGame::~BombermanGame() = default;

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

	m_GameStateMachine = std::make_unique<GameStateMachine>();
	m_GameStateMachine->SetInitialState(std::make_unique<MainMenuGameState>(*this));
}

void bombGame::BombermanGame::Update(float deltaTime)
{
	m_GameStateMachine->Update(deltaTime);
}

void bombGame::BombermanGame::FixedUpdate(float fixedDeltaSec)
{
	m_GameStateMachine->FixedUpdate(fixedDeltaSec);
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
	return *m_GameStateMachine;
}

void bombGame::BombermanGame::LoadSound()
{
	m_StoredSoundSystem->RegisterSound(SoundIds::ExplosionBomb, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomb_explosion.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::BombermanDied, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomberman_killed.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::LayBomb, ge::ResourceManager::GetInstance().GetFullPath("sounds/bomb_lay.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::Pause, ge::ResourceManager::GetInstance().GetFullPath("sounds/pause.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::Powerup, ge::ResourceManager::GetInstance().GetFullPath("sounds/powerup.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::Step_Horizontal, ge::ResourceManager::GetInstance().GetFullPath("sounds/step_horizontal.wav"));
	m_StoredSoundSystem->RegisterSound(SoundIds::Step_Vertical, ge::ResourceManager::GetInstance().GetFullPath("sounds/step_vertical.wav"));
}
