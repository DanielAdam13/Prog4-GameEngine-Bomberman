#include "LossState.h"

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
#include "Renderer.h"

#include <utility>
#include <SDL3/SDL_pixels.h>
#include <string>

bombGame::LossState::LossState(BombermanGame& game, float duration)
	:GameState::GameState(game),
	m_LossScreenDuration{ duration }
{
}

void bombGame::LossState::OnEnter()
{
	// Resources
	const auto victoryTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_LossScreen.png") };

	const auto bigTitleFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 70) };
	bigTitleFont->SetBold(true);
	//constexpr SDL_Color colorYellow{ SDL_Color{220, 220, 60, 255} };
	constexpr SDL_Color colorRed{ SDL_Color{230, 30, 40, 255} };

	const auto designSize{ ge::Renderer::GetInstance().GetWindowDesignSize() };

	// ---------------------
	// Initialize Scene
	// ---------------------
	ge::Scene& lossScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::Loss) };

	// Static object initalization:
	auto lossScreenGO = std::make_unique<ge::GameObject>("GO_VictoryScreen");
	lossScreenGO->AddComponent<ge::Image>(lossScreenGO.get())->SetTexture(victoryTexture);
	lossScreenGO->GetComponent<ge::Transform>()->SetLocalScale(0.7f, 0.8f, 1.f);
	lossScene.Add(std::move(lossScreenGO));

	auto scoreGO = std::make_unique<ge::GameObject>("GO_ScoreText");
	scoreGO->AddComponent<ge::TextComponent>(scoreGO.get(),
		std::to_string(GetBombermanGame().GetCurrentGameSession().totalScore), bigTitleFont, colorRed);
	scoreGO->GetComponent<ge::Transform>()->SetLocalPosition({ 
		designSize.first * 0.35f, designSize.second * 0.55f, 0.f });
	lossScene.Add(std::move(scoreGO));

	// !! Save High Score !!
	GetBombermanGame().SaveEntryToHighScore();

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
		return std::make_unique<MainMenuGameState>(GetBombermanGame());
	}

	return nullptr;
}
