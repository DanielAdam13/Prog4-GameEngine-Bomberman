#include "HighScoresState.h"
#include "BombermanGame.h"
#include "SoundManager.h"
#include "Commands/SwitchToMainMenuCommand.h"
#include "Components/Image.h"

#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Components/TextComponent.h"
#include "Components/Transform.h"
#include "Services/ServiceLocator.h"
#include "Services/InputManager.h"

#include <vector>
#include <utility>
#include <string>
#include <SDL3/SDL_scancode.h>

bombGame::HighScoreState::HighScoreState(BombermanGame& game)
	:GameState::GameState(game)
{
}

void bombGame::HighScoreState::OnEnter()
{
    auto& highScene{ ge::SceneManager::GetInstance().CreateScene(sceneNames::HighScores) };
   
    // --- Resources ---
    const auto blackTexture{ ge::ResourceManager::GetInstance().LoadTexture("sprites/I_BlackScreen.png") };

    const auto font{ ge::ResourceManager::GetInstance().LoadFont("fonts/Lingua.otf", 40) };
    constexpr SDL_Color colorWhite{ 220, 220, 220, 255 };
    constexpr SDL_Color colorYellow{ 220, 220, 60, 255 };

    const auto designSize{ ge::Renderer::GetInstance().GetWindowDesignSize() };

    // BG
    auto blackBackgroundGO = std::make_unique<ge::GameObject>("GO_BlackBacgkround");
    blackBackgroundGO->AddComponent<ge::Image>(blackBackgroundGO.get())->SetTexture(blackTexture);
    blackBackgroundGO->GetComponent<ge::Transform>()->SetLocalScale(0.7f, 0.8f, 1.f);
    highScene.Add(std::move(blackBackgroundGO));

    // Title
    auto titleGO = std::make_unique<ge::GameObject>("GO_HighScoreTitle");
    titleGO->AddComponent<ge::TextComponent>(titleGO.get(), "HIGH SCORES", font, colorYellow);
    titleGO->GetComponent<ge::Transform>()->SetLocalPosition({
        designSize.first * 0.3f, designSize.second * 0.1f, 0.f });
    highScene.Add(std::move(titleGO));

    // Entries
    const auto& entries{ GetBombermanGame().GetHighScoreList().GetEntries() };
    const float baseY{ designSize.second * 0.25f };
    constexpr float rowSpacing{ 50.f };

    for (size_t i{}; i < entries.size(); ++i) 
    {
        const std::string row{
            std::to_string(i + 1) + ".  " +
            entries[i].name + "   " +
            std::to_string(entries[i].score) };

        auto rowGO = std::make_unique<ge::GameObject>("GO_HighScoreRow" + std::to_string(i));
        rowGO->AddComponent<ge::TextComponent>(rowGO.get(), row, font, colorWhite);
        rowGO->GetComponent<ge::Transform>()->SetLocalPosition({
            designSize.first * 0.25f, baseY + i * rowSpacing, 0.f });
        highScene.Add(std::move(rowGO));
    }

    // Input: any key returns to main menu
    auto& im{ ge::ServiceLocator::GetInputManager() };

    im.BindKeyboardCommand(SDL_SCANCODE_E, ge::InputManager::InputTrigger::Up,
        std::make_unique<SwitchToMainMenuCommand>(GetBombermanGame()));
    im.BindControllerCommand(ge::ControllerButton::A, ge::InputManager::InputTrigger::Up,
        std::make_unique<SwitchToMainMenuCommand>(GetBombermanGame()));

    ge::SceneManager::GetInstance().SwitchToSceneWithName(sceneNames::HighScores);
}

void bombGame::HighScoreState::OnExit()
{
    GetBombermanGame().GetSoundManager().StopAllSounds();
    ge::SceneManager::GetInstance().RemoveSceneWithName(sceneNames::HighScores);
}
