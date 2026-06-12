#include "TypeNameState.h"
#include "BombermanGame.h"
#include "Components/SelectableTextComponent.h"
#include "Commands/HoverSelectableCommand.h"
#include "Commands/ConfirmSelectionCommand.h"
#include "SoundManager.h"
#include "MainMenuState.h"
#include "Commands/ChangeWindowSizeCommand.h"
#include "Commands/ToggleMuteCommand.h"
#include "SoundIds.h"
#include "Services/SoundSystem.h"

#include "ResourceManager.h"
#include "Renderer.h"
#include "Font.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Components/TextComponent.h"
#include "Components/Transform.h"
#include "Services/ServiceLocator.h"
#include "Services/InputManager.h"
#include "GameObject.h"
#include "Components/Image.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_scancode.h>

bombGame::TypeNameState::TypeNameState(BombermanGame& game)
	:GameState::GameState(game)
{
}

void bombGame::TypeNameState::OnEnter()
{
	// Resources
	const auto nameSelectionTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_NameSelection.png") };

	const auto selectionFont{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 40) };
	selectionFont->SetBold(true);

	constexpr SDL_Color colorWhite{ SDL_Color{220, 220, 220, 255} };
	constexpr SDL_Color colorYellow{ SDL_Color{220, 220, 60, 255} };

	const auto designSize{ ge::Renderer::GetInstance().GetWindowDesignSize() };

	// ---------------------
	// Initialize Scene
	// ---------------------
	ge::Scene& textSelectionScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::NameSelection) };

	auto nameScreenGO = std::make_unique<ge::GameObject>("GO_SelectionScreen");
	nameScreenGO->AddComponent<ge::Image>(nameScreenGO.get())->SetTexture(nameSelectionTexture);
	nameScreenGO->GetComponent<ge::Transform>()->SetLocalScale(0.7f, 0.8f, 1.f);
	textSelectionScene.Add(std::move(nameScreenGO));

	// Object initalization:
	const float baseX{ designSize.first * 0.3f };
	const float baseY{ designSize.second * 0.5f };
	constexpr float slotSpacing{ 60.f };

	// Letters
	for (size_t i{}; i < LetterCount; ++i)
	{
		auto letterGO = std::make_unique<ge::GameObject>("GO_LetterSlot" + std::to_string( i ));
		letterGO->GetComponent<ge::Transform>()->SetLocalPosition(
			{ baseX + i * slotSpacing, baseY, 0.f });
		letterGO->AddComponent<ge::TextComponent>(letterGO.get(),
			std::string(1, m_Letters[i]), selectionFont, colorWhite);

		auto* selComp{ letterGO->AddComponent<SelectableTextComponent>(letterGO.get(), colorWhite, colorYellow, nullptr) };
		m_Selectables.push_back(selComp);
		textSelectionScene.Add(std::move(letterGO));
	}

	// Confirm arrow slot
	auto arrowGO = std::make_unique<ge::GameObject>("GO_ConfirmArrow");
	arrowGO->GetComponent<ge::Transform>()->SetLocalPosition({
		baseX + LetterCount * slotSpacing + 30.f, baseY, 0.f });
	arrowGO->AddComponent<ge::TextComponent>(arrowGO.get(), "->", selectionFont, colorWhite);

	auto* arrowSel{ arrowGO->AddComponent<SelectableTextComponent>(
		arrowGO.get(), colorWhite, colorYellow,
		nullptr) };   // state's ConfirmCurrentSelection handles the transition
	m_Selectables.push_back(arrowSel);
	textSelectionScene.Add(std::move(arrowGO));
	
	// --- Initial hover ---
	m_CurrentHoverIndex = 0;
	m_Selectables[m_CurrentHoverIndex]->OnHover();

	// ---------------------
	// Input Binding
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
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(0, -1)));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_S, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(0, 1)));
	inputManager.BindKeyboardCommand(SDL_SCANCODE_E, ge::InputManager::InputTrigger::Up,
		std::make_unique<ConfirmSelectionCommand>(this));

	inputManager.BindControllerCommand(ge::ControllerButton::DpadLeft, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(-1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadRight, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(1, 0)));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadUp, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(0, -1)));
	inputManager.BindControllerCommand(ge::ControllerButton::DpadDown, ge::InputManager::InputTrigger::Up,
		std::make_unique<HoverSelectableCommand>(this, std::make_pair<int, int>(0, 1)));
	inputManager.BindControllerCommand(ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
		std::make_unique<ConfirmSelectionCommand>(this));

	ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::NameSelection);
}

void bombGame::TypeNameState::OnExit()
{
	auto& im{ ge::ServiceLocator::GetInputManager() };
	im.UnbindAllKeyboard();
	im.UnbindAllController();

	GetBombermanGame().GetSoundManager().StopAllSounds();
	ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::NameSelection);
}

void bombGame::TypeNameState::MoveHover(std::pair<int, int> delta)
{
	if (m_Selectables.empty())
		return;

	// --- Horizontal Hover -> change slot ---
	if (delta.first != 0 && delta.second == 0)
	{
		const int newIndex{ static_cast<int>(m_CurrentHoverIndex) + delta.first };
		if (newIndex < 0 || newIndex > static_cast<int>(m_Selectables.size() - 1))
			return;

		m_Selectables[m_CurrentHoverIndex]->OnExit();
		m_CurrentHoverIndex = static_cast<size_t>(newIndex);
		m_Selectables[m_CurrentHoverIndex]->OnHover();

		// Play Sound
		GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::StepHorizontal, 0.3f, ge::SoundCategory::SFX);

		return;
	}

	// --- Vertical Hover -> cycle letter at hovered slot ---
	if (delta.first == 0 && delta.second != 0)
	{
		if (!IsLetterSlot(m_CurrentHoverIndex)) 
			return;

		char& letter{ m_Letters[m_CurrentHoverIndex] };
		letter += static_cast<char>(delta.second);

		if (letter > 'Z')
			letter = 'A';
		else if (letter < 'A')
			letter = 'Z';

		m_Selectables[m_CurrentHoverIndex]->SetText(std::string(1, letter));

		// Play Sound
		GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::StepVertical, 0.3f, ge::SoundCategory::SFX);
	}
}

void bombGame::TypeNameState::ConfirmCurrentSelection()
{
	if (m_Selectables.empty())
		return;

	m_Selectables[m_CurrentHoverIndex]->OnSelected(); // visual only

	// Do nothing for letter slots
	if (m_CurrentHoverIndex != ConfirmSlotIndex)
		return;

	std::string finalName(m_Letters.begin(), m_Letters.end());
	GetBombermanGame().SetPlayerName(finalName);

	// Transition to MainMenu
	GetBombermanGame().GetStateMachine().RequestStateTransition(
		std::make_unique<MainMenuGameState>(GetBombermanGame()));
}

bool bombGame::TypeNameState::IsLetterSlot(size_t index) const
{
	return index < LetterCount;
}
