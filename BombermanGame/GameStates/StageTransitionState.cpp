#include "StageTransitionState.h"
#include "BombermanGame.h"
#include "SoundIds.h"
#include "GameplayState.h"

#include "SceneManager.h"
#include "Services/SoundSystem.h" // for sound channels enum class
#include "GameObject.h"
#include "Components/TextComponent.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "ResourceManager.h"
#include "Font.h"
#include "Renderer.h" // for window size

#include <string>
#include <utility>
#include <SDL3/SDL_pixels.h>

bombGame::StageTransitionState::StageTransitionState(BombermanGame& game, float duration)
	:GameState::GameState(game),
	m_StageIndex{ game.GetCurrentGameSession().currentStageIndex },
	m_TransitionDuration{ duration }
{
}

void bombGame::StageTransitionState::OnEnter()
{
	auto& transitionScene = ge::SceneManager::GetInstance().CreateScene(sceneNames::Transition);

	// Resources
	const auto blackTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_BlackScreen.png") };

	constexpr SDL_Color colorGray{ SDL_Color{200, 200, 200, 255} };
	const auto stageFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 40) };
	stageFont->SetBold(true);
	const auto tutFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 25) };
	stageFont->SetBold(true);

	// Game Object(s)
	const auto windowSize{ ge::Renderer::GetInstance().GetWindowSize() };

	auto blackBackgroundGO = std::make_unique<ge::GameObject>("GO_BlackBacgkround");
	blackBackgroundGO->AddComponent<ge::Image>(blackBackgroundGO.get())->SetTexture(blackTexture);
	blackBackgroundGO->GetComponent<ge::Transform>()->SetLocalScale(0.7f, 0.8f, 1.f);
	transitionScene.Add(std::move(blackBackgroundGO));

	auto textGO = std::make_unique<ge::GameObject>("GO_StageText");
	textGO->AddComponent<ge::TextComponent>(textGO.get(),
		"STAGE " + std::to_string(m_StageIndex + 1), stageFont, colorGray);
	transitionScene.Add(std::move(textGO));

	//  --- Tutorial Text ---
	auto tutorial1GO = std::make_unique<ge::GameObject>("GO_TutorialText1");
	tutorial1GO->AddComponent<ge::TextComponent>(tutorial1GO.get(),
		"D-Pad to move Player 1 | A bomb | B detonate(if)", tutFont, colorGray);
	tutorial1GO->GetComponent<ge::Transform>()->SetLocalPosition(
		glm::vec3{ windowSize.first * 0.05f, windowSize.second * 0.7f, 0.f });
	tutorial1GO->SetIgnoreCamera(true);
	transitionScene.Add(std::move(tutorial1GO));

	auto tutorial2GO = std::make_unique<ge::GameObject>("GO_TutorialText2");
	tutorial2GO->AddComponent<ge::TextComponent>(tutorial2GO.get(),
		"WASD to move Player 2 | SPACE bomb | X detonate(if) ", tutFont, colorGray);
	tutorial2GO->GetComponent<ge::Transform>()->SetLocalPosition(
		glm::vec3{ windowSize.first * 0.05f, windowSize.second * 0.8f, 0.f });
	tutorial2GO->SetIgnoreCamera(true);
	transitionScene.Add(std::move(tutorial2GO));

	auto tutorial3GO = std::make_unique<ge::GameObject>("GO_TutorialText3");
	tutorial3GO->AddComponent<ge::TextComponent>(tutorial3GO.get(),
		"F2 mute | F1 skip | R reset ", tutFont, colorGray);
	tutorial3GO->GetComponent<ge::Transform>()->SetLocalPosition(
		glm::vec3{ windowSize.first * 0.6f, windowSize.second * 0.05f, 0.f });
	tutorial3GO->SetIgnoreCamera(true);
	transitionScene.Add(std::move(tutorial3GO));
	
	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::StageEnter, 0.3f, ge::SoundCategory::Music);
	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::Transition);
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
