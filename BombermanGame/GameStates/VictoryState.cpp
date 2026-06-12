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
#include "Components/TextComponent.h"
#include "Font.h"
#include "Renderer.h"

#include <utility>
#include <string>
#include <SDL3/SDL_pixels.h>

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

	const auto bigTitleFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 70) };
	bigTitleFont->SetBold(true);

	constexpr SDL_Color colorYellow{ SDL_Color{220, 220, 60, 255} };
	//constexpr SDL_Color colorRed{ SDL_Color{230, 30, 40, 255} };

	const auto windowSize{ ge::Renderer::GetInstance().GetWindowSize() };

	// Static object initalization:
	auto victoryScreenGO = std::make_unique<ge::GameObject>("GO_VictoryScreen");
	victoryScreenGO->AddComponent<ge::Image>(victoryScreenGO.get())->SetTexture(victoryTexture);
	victoryScreenGO->GetComponent<ge::Transform>()->SetLocalScale(0.7f, 0.8f, 1.f);
	victoryScene.Add(std::move(victoryScreenGO));

	auto scoreGO = std::make_unique<ge::GameObject>("GO_ScoreText");
	scoreGO->AddComponent<ge::TextComponent>(scoreGO.get(),
		std::to_string(GetBombermanGame().GetCurrentGameSession().totalScore), bigTitleFont, colorYellow);
	scoreGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.35f, windowSize.second * 0.55f, 0.f });
	victoryScene.Add(std::move(scoreGO));

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
		return std::make_unique<MainMenuGameState>(GetBombermanGame());
	}

	return nullptr;
}
