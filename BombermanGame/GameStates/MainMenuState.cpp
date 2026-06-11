#include "MainMenuState.h"

#include "SceneManager.h"
#include "Scene.h"
#include "BombermanGame.h"
#include "Services/ServiceLocator.h"
#include "Services/SoundSystem.h" // for sound channels enum class
#include "Renderer.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "SoundIds.h"

//#include "Commands/ChangeWindowSizeCommand.h"
#include "Commands/ToggleMuteCommand.h"

#include "GameObject.h"
#include "Components/Image.h"
#include "Components/Transform.h"
#include "Commands/SwitchToTransitionStateCommand.h"

#include <utility>
#include <memory>

bombGame::MainMenuGameState::MainMenuGameState(BombermanGame& game)
	:GameState::GameState(game)
{
}

void bombGame::MainMenuGameState::OnEnter()
{
	// ---------------------
	// Load Resources
	// ---------------------
	const auto mainMenuTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_MainMenu1.png") };
	
	// ---------------------
	// Initialize Scene
	// ---------------------
	ge::Scene& mainMenuScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::MainMenu) };

	// Static object initalization:
	auto backgroundGO = std::make_unique<ge::GameObject>("GO_MainMenuBG");
	backgroundGO->AddComponent<ge::Image>(backgroundGO.get())->SetTexture(mainMenuTexture);
	backgroundGO->GetComponent<ge::Transform>()->SetLocalScale(3.f, 3.3f, 1.f);
	mainMenuScene.Add(std::move(backgroundGO));

	// ---------------------
	// Specify Game State Input Bindings
	// ---------------------
	auto& inputManager{ ge::ServiceLocator::GetInputManager() };

	/*inputManager.BindKeyboardCommand(SDL_SCANCODE_F11, ge::InputManager::InputTrigger::Up,
		std::make_unique<ge::ChangeWindowSizeCommand>(1200, 1200));*/
	inputManager.BindKeyboardCommand(SDL_SCANCODE_F2, ge::InputManager::InputTrigger::Up,
		std::make_unique<ToggleMuteCommand>(GetBombermanGame()));

	inputManager.BindKeyboardCommand(SDL_SCANCODE_E, ge::InputManager::InputTrigger::Up,
		std::make_unique<SwitchToTransitionCommand>(GetBombermanGame()));
	inputManager.BindControllerCommand(ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
		std::make_unique<SwitchToTransitionCommand>(GetBombermanGame()));

	// Play Sound
	//GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::MainMenuOST, 0.3f, ge::SoundCategory::Music);

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
