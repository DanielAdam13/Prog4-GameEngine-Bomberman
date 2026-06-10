#include "VictoryState.h"
#include "MainMenuState.h"
#include "BombermanGame.h"
#include "SoundIds.h"

#include "Scene.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Services/SoundSystem.h"
#include "Components/Image.h"
#include "Components/Transform.h"

#include <utility>

bombGame::VictoryState::VictoryState(BombermanGame& game, float duration)
	:GameState::GameState(game),
	m_VictoryScreenDuration{ duration }
{
}

void bombGame::VictoryState::OnEnter()
{
	// Resources
	const auto victoryTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_VictoryScreen.png") };

	// ---------------------
	// Initialize Scene
	// ---------------------
	ge::Scene& victoryScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::Victory) };

	// Static object initalization:
	auto victoryScreenGO = std::make_unique<ge::GameObject>("GO_VictoryScreen");
	victoryScreenGO->AddComponent<ge::Image>(victoryScreenGO.get())->SetTexture(victoryTexture);
	victoryScreenGO->GetComponent<ge::Transform>()->SetLocalScale(0.75f, 0.75f, 1.f);
	victoryScene.Add(std::move(victoryScreenGO));

	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::StageWon, 0.3f, ge::SoundCategory::Music);
	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::Victory);
}

void bombGame::VictoryState::OnExit()
{
	GetBombermanGame().GetSoundManager().StopAllSounds();
	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::Victory);
}

std::unique_ptr<bombGame::GameState> bombGame::VictoryState::Update(float deltaTime)
{
	m_VictoryTimer += deltaTime;
	if (m_VictoryTimer >= m_VictoryScreenDuration)
	{
		GetBombermanGame().GetCurrentGameSession().currentStageIndex = 0;
		return std::make_unique<MainMenuGameState>(GetBombermanGame());
	}

	return nullptr;
}
