#include "LossState.h"

#include "MainMenuState.h"
#include "BombermanGame.h"
#include "SoundIds.h"

#include "Scene.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Services/SoundSystem.h"
#include "Components/TextComponent.h"
#include "Components/Transform.h"
#include "Font.h"

#include <utility>
#include <SDL3/SDL_pixels.h>

bombGame::LossState::LossState(BombermanGame& game, float duration)
	:GameState::GameState(game),
	m_LossScreenDuration{ duration }
{
}

void bombGame::LossState::OnEnter()
{
	// Resources
	const auto lossFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 50) };
	lossFont->SetBold(true);

	//constexpr SDL_Color colorBlack{ SDL_Color{0, 0, 0, 255} };
	constexpr SDL_Color colorRed{ SDL_Color{215, 40, 30, 255} };

	// ---------------------
	// Initialize Scene
	// ---------------------
	ge::Scene& victoryScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::Loss) };

	// Static object initalization:
	auto lossTextGO = std::make_unique<ge::GameObject>("GO_LossText");
	lossTextGO->AddComponent<ge::TextComponent>(lossTextGO.get(), "YOU LOST", lossFont, colorRed);
	lossTextGO->GetComponent<ge::Transform>()->SetLocalPosition({300.f, 400.f, 0.f});
	victoryScene.Add(std::move(lossTextGO));

	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::GameLost, 0.3f, ge::SoundCategory::Music);
	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::Loss);
}

void bombGame::LossState::OnExit()
{
	GetBombermanGame().GetSoundManager().StopAllSounds();
	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::Loss);
}

std::unique_ptr<bombGame::GameState> bombGame::LossState::Update(float deltaTime)
{
	m_LossTimer += deltaTime;
	if (m_LossTimer >= m_LossScreenDuration)
	{
		m_LossTimer = 0.f;
		GetBombermanGame().GetCurrentGameSession().currentStageIndex = 0;
		return std::make_unique<MainMenuGameState>(GetBombermanGame());
	}

	return nullptr;
}
