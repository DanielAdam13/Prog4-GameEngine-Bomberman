#include "SwitchToGameplaySceneCommand.h"
#include "SceneManager.h"
#include "Services/ServiceLocator.h"
#include "Services/InputManager.h"

#include <utility>

bombGame::SwitchToGamplayCommand::SwitchToGamplayCommand(const std::string& gameplaySceneName)
	:Command::Command(),
	m_GameplaySceneName{ gameplaySceneName }
{
}

void bombGame::SwitchToGamplayCommand::Execute(float)
{
	ge::SceneManager::GetInstance().SwitchToSceneWithName(m_GameplaySceneName);
}
