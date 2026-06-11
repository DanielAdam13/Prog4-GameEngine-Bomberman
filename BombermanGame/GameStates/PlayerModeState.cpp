#include "PlayerModeState.h"
#include "BombermanGame.h"
#include "Components/SelectableTextComponent.h"
#include "Commands/HoverSelectableCommand.h"
#include "Commands/ConfirmSelectionCommand.h"
#include "TypeNameState.h"

#include "ResourceManager.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Components/TextComponent.h"
#include "Components/Transform.h"
#include "SoundIds.h"
#include "Services/SoundSystem.h"
#include "GameObject.h"
#include "Services/ServiceLocator.h"
#include "Services/InputManager.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_scancode.h>

bombGame::PlayerModeState::PlayerModeState(BombermanGame& game)
	:GameState::GameState(game)
{
}

void bombGame::PlayerModeState::OnEnter()
{
	// Resources
	const auto bigTitleFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 80) };
	bigTitleFont->SetBold(true);
	const auto smallTitleFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 60) };
	//smallTitleFont->SetBold(true);
	const auto selectionFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 30) };
	selectionFont->SetBold(true);

	constexpr SDL_Color colorWhite{ SDL_Color{220, 220, 220, 255} };
	constexpr SDL_Color colorYellow{ SDL_Color{220, 220, 60, 255} };
	constexpr SDL_Color colorRed{ SDL_Color{230, 30, 40, 255} };

	const auto windowSize{ ge::Renderer::GetInstance().GetWindowSize() };

	// ---------------------
	// Initialize Scene
	// ---------------------
	ge::Scene& playerModeScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::PlayerModeSelection) };

	// Object initalization:
	auto selectionTitleGO1 = std::make_unique<ge::GameObject>("GO_SelectionTitle1");
	selectionTitleGO1->AddComponent<ge::TextComponent>(selectionTitleGO1.get(), "SELECT", bigTitleFont, colorRed);
	selectionTitleGO1->GetComponent<ge::Transform>()->SetLocalPosition({ 230.f, windowSize.second * 0.1f, 0.f });
	playerModeScene.Add(std::move(selectionTitleGO1));

	auto selectionTitleGO2 = std::make_unique<ge::GameObject>("GO_SelectionTitle2");
	selectionTitleGO2->AddComponent<ge::TextComponent>(selectionTitleGO2.get(), "YOUR PLAY MODE", smallTitleFont, colorRed);
	selectionTitleGO2->GetComponent<ge::Transform>()->SetLocalPosition({ 150.f, windowSize.second * 0.2f, 0.f });
	playerModeScene.Add(std::move(selectionTitleGO2));

	auto singlePlayerModeGO = std::make_unique<ge::GameObject>("GO_SingleChoice");
	singlePlayerModeGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.1f, windowSize.second * 0.5f, 0.f });
	singlePlayerModeGO->AddComponent<ge::TextComponent>(singlePlayerModeGO.get(), "SinglePlayer", selectionFont, colorWhite);
	auto* singleSelComp{ singlePlayerModeGO->AddComponent<SelectableTextComponent>(singlePlayerModeGO.get(), colorWhite, colorYellow,
		[this]() -> void
		{
			GetBombermanGame().SetPlayerMode(PlayerMode::SinglePlayer);
		}) };
	m_Selectables.push_back(singleSelComp);
	playerModeScene.Add(std::move(singlePlayerModeGO));

	auto coopModeGO = std::make_unique<ge::GameObject>("GO_CoopChoice");
	coopModeGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.42f, windowSize.second * 0.7f, 0.f });
	coopModeGO->AddComponent<ge::TextComponent>(coopModeGO.get(), "Co-op", selectionFont, colorWhite);
	auto* coopSelComp{ coopModeGO->AddComponent<SelectableTextComponent>(coopModeGO.get(), colorWhite, colorYellow,
		[this]() -> void
		{
			GetBombermanGame().SetPlayerMode(PlayerMode::Coop);
		}) };
	m_Selectables.push_back(coopSelComp);
	playerModeScene.Add(std::move(coopModeGO));

	auto versusModeGO = std::make_unique<ge::GameObject>("GO_VersusChoice");
	versusModeGO->GetComponent<ge::Transform>()->SetLocalPosition({
		windowSize.first * 0.75f, windowSize.second * 0.5f, 0.f });
	versusModeGO->AddComponent<ge::TextComponent>(versusModeGO.get(), "Versus", selectionFont, colorWhite);
	auto* versusSelComp{ versusModeGO->AddComponent<SelectableTextComponent>(versusModeGO.get(), colorWhite, colorYellow,
		[this]() -> void
		{
			GetBombermanGame().SetPlayerMode(PlayerMode::Versus);
		}) };
	m_Selectables.push_back(versusSelComp);
	playerModeScene.Add(std::move(versusModeGO));

	// --- Initial hover ---
	m_CurrentHoverIndex = 0;
	m_Selectables[m_CurrentHoverIndex]->OnHover();

	// ---------------------
	// Input Binding
	// ---------------------
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };

	inputManager.BindKeyboardCommand(SDL_SCANCODE_A, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(- 1, 0 )));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_D, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(1, 0)));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_E, ge::InputManager::InputTrigger::Up,
		std::make_unique<ConfirmSelectionCommand>(this));

	inputManager.BindControllerCommand(ge::ControllerButton::DpadLeft, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(-1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadRight, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
		std::make_unique<ConfirmSelectionCommand>(this));

	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::MainMenuOST, 0.3f, ge::SoundCategory::Music);
	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::PlayerModeSelection);
}

void bombGame::PlayerModeState::OnExit()
{
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };
	inputManager.UnbindAllController();
	inputManager.UnbindAllKeyboard();

	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::PlayerModeSelection);
}

void bombGame::PlayerModeState::MoveHover(std::pair<int, int> delta)
{
	if (m_Selectables.empty())
		return;

	const int newIndex{ static_cast<int>(m_CurrentHoverIndex) + delta.first };
	if(newIndex < 0 || newIndex > static_cast<int>(m_Selectables.size() - 1))
		return;

	m_Selectables[m_CurrentHoverIndex]->OnExit();
	m_CurrentHoverIndex = static_cast<size_t>(newIndex);
	m_Selectables[m_CurrentHoverIndex]->OnHover();
}

void bombGame::PlayerModeState::ConfirmCurrentSelection()
{
	if (m_Selectables.empty())
		return;

	m_Selectables[m_CurrentHoverIndex]->OnSelected();

	// Transition to MainMenu
	GetBombermanGame().GetStateMachine().RequestStateTransition(
		std::make_unique<TypeNameState>(GetBombermanGame()));
}
