#include "MainMenuState.h"
#include "BombermanGame.h"
#include "SoundManager.h"
#include "SoundIds.h"
#include "Commands/ToggleMuteCommand.h"
#include "Commands/ChangeWindowSizeCommand.h"
#include "Components/SelectableTextComponent.h"
#include "Commands/HoverSelectableCommand.h"
#include "Commands/ConfirmSelectionCommand.h"
#include "StageTransitionState.h"
#include "HighScoresState.h"

#include "GameObject.h"
#include "Components/Image.h"
#include "Components/Transform.h"
#include "Components/TextComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Services/ServiceLocator.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Services/SoundSystem.h" // for sound channels enum class

#include <utility>
#include <memory>

bombGame::MainMenuGameState::MainMenuGameState(BombermanGame& game)
	:GameState::GameState(game)
{
}

void bombGame::MainMenuGameState::OnEnter()
{
	GetBombermanGame().ClearGameSession();

	// ---------------------
	// Load Resources
	// ---------------------
	const auto mainMenuTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_MainMenu1.png") };

	const auto selectionFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 50) };
	selectionFont->SetBold(true);
	constexpr SDL_Color colorWhite{ SDL_Color{220, 220, 220, 255} };
	constexpr SDL_Color colorYellow{ SDL_Color{220, 220, 60, 255} };

	const auto designSize{ ge::Renderer::GetInstance().GetWindowDesignSize() };
	
	// ---------------------
	// Initialize Scene
	// ---------------------
	ge::Scene& mainMenuScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::MainMenu) };

	// Static object initalization:
	// Background
	auto backgroundGO = std::make_unique<ge::GameObject>("GO_MainMenuBG");
	backgroundGO->AddComponent<ge::Image>(backgroundGO.get())->SetTexture(mainMenuTexture);
	backgroundGO->GetComponent<ge::Transform>()->SetLocalScale(3.1f, 3.1f, 1.f);
	mainMenuScene.Add(std::move(backgroundGO));

	// Selectable text
	auto startChoiceGO = std::make_unique<ge::GameObject>("GO_StartChoice");
	startChoiceGO->GetComponent<ge::Transform>()->SetLocalPosition({
		designSize.first * 0.4f, designSize.second * 0.65f, 0.f });
	startChoiceGO->AddComponent<ge::TextComponent>(startChoiceGO.get(), "START", selectionFont, colorWhite);
	auto* startSelComp{ startChoiceGO->AddComponent<SelectableTextComponent>(startChoiceGO.get(), colorWhite, colorYellow,
		[this]() -> void
		{
			// Manual Switch
			GetBombermanGame().GetStateMachine().RequestStateTransition(
				std::make_unique<StageTransitionState>(GetBombermanGame()));
		}) };
	m_Selectables.push_back(startSelComp);
	mainMenuScene.Add(std::move(startChoiceGO));

	auto highScoreChoiceGO = std::make_unique<ge::GameObject>("GO_HighScoresChoice");
	highScoreChoiceGO->GetComponent<ge::Transform>()->SetLocalPosition({
		designSize.first * 0.3f, designSize.second * 0.77f, 0.f });
	highScoreChoiceGO->AddComponent<ge::TextComponent>(highScoreChoiceGO.get(), "HIGH SCORES", selectionFont, colorWhite);
	auto* highSelComp{ highScoreChoiceGO->AddComponent<SelectableTextComponent>(highScoreChoiceGO.get(), colorWhite, colorYellow,
		[this]() -> void
		{
			// Manual Switch
			GetBombermanGame().GetStateMachine().RequestStateTransition(
				std::make_unique<HighScoreState>(GetBombermanGame()));
		}) };
	m_Selectables.push_back(highSelComp);
	mainMenuScene.Add(std::move(highScoreChoiceGO));

	// --- Initial hover ---
	m_CurrentHoverIndex = 0;
	m_Selectables[m_CurrentHoverIndex]->OnHover();

	// ---------------------
	// Specify Game State Input Bindings
	// ---------------------
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };

	inputManager.BindKeyboardCommand(SDL_SCANCODE_F11, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ChangeWindowSizeCommand>(1200, 1200));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_F2, ge::InputManager::InputTrigger::Up,
		std::make_unique<ToggleMuteCommand>(GetBombermanGame()));

	inputManager.BindKeyboardCommand(SDL_SCANCODE_A, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(-1, 0)));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_D, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(1, 0)));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_W, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(-1, 0)));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_S, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(1, 0)));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_E, ge::InputManager::InputTrigger::Up,
		std::make_unique<ConfirmSelectionCommand>(this));

	inputManager.BindControllerCommand(ge::ControllerButton::DpadLeft, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(-1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadRight, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadUp, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(-1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadDown, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
		std::make_unique<ConfirmSelectionCommand>(this));

	// Play Sound
	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::MainMenuOST, 0.2f, ge::SoundCategory::Music);

	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::MainMenu);
}

void bombGame::MainMenuGameState::OnExit()
{
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };
	inputManager.UnbindAllController();
	inputManager.UnbindAllKeyboard();

	GetBombermanGame().GetSoundManager().StopAllSounds();
	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::MainMenu);
}

void bombGame::MainMenuGameState::MoveHover(std::pair<int, int> delta)
{
	if (m_Selectables.empty())
		return;

	const int newIndex{ static_cast<int>(m_CurrentHoverIndex) + delta.first };
	if (newIndex < 0 || newIndex > static_cast<int>(m_Selectables.size() - 1))
		return;

	m_Selectables[m_CurrentHoverIndex]->OnExit();
	m_CurrentHoverIndex = static_cast<size_t>(newIndex);
	m_Selectables[m_CurrentHoverIndex]->OnHover();

	// Play Sound
	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::StepVertical, 0.3f, ge::SoundCategory::SFX);
}

void bombGame::MainMenuGameState::ConfirmCurrentSelection()
{
	if (m_Selectables.empty())
		return;

	m_Selectables[m_CurrentHoverIndex]->OnSelected();
}
