#include "StageTransitionState.h"
#include "BombermanGame.h"
#include "SoundIds.h"
#include "GameplayState.h"

#include "SceneManager.h"
#include "Services/SoundSystem.h" // for sound channels enum class
#include "GameObject.h"
#include "Components/TextComponent.h"
#include "ResourceManager.h"
#include "Font.h"

#include <string>
#include <utility>

bombGame::StageTransitionState::StageTransitionState(BombermanGame& game, float duration)
	:GameState::GameState(game),
	m_StageIndex{ game.GetCurrentGameSession().currentStageIndex },
	m_TransitionDuration{ duration }
{
}

void bombGame::StageTransitionState::OnEnter()
{
	auto& scene = ge::SceneManager::GetInstance().CreateScene(sceneNames::Transition);

	// Resources
	constexpr SDL_Color colorBlue{ SDL_Color{200, 200, 200, 255} };
	const auto font{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 40) };
	font->SetBold(true);

	// Game Object(s)
	auto textGO = std::make_unique<ge::GameObject>("GO_StageText");
	textGO->AddComponent<ge::TextComponent>(textGO.get(),
		"STAGE " + std::to_string(m_StageIndex + 1), font, colorBlue);
	scene.Add(std::move(textGO));

	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::Transition);
	
	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::StageEnter, 0.3f, ge::SoundCategory::Music);
}

void bombGame::StageTransitionState::OnExit()
{
	GetBombermanGame().GetSoundManager().StopAllSounds();
	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::Transition);
}

std::unique_ptr<bombGame::GameState> bombGame::StageTransitionState::Update(float deltaTime)
{
	m_Timer += deltaTime;
	if (m_Timer >= m_TransitionDuration)
	{
		return std::make_unique<GameplayGameState>(GetBombermanGame());
	}

	return nullptr;
}
