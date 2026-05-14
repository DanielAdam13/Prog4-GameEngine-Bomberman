#include "SwitchToGameplayStateCommand.h"

#include "BombermanGame.h"
#include "GameStates/GameStateMachine.h"
#include "GameStates/GameplayState.h"

#include <utility>
#include <memory>

bombGame::SwitchToGameplayCommand::SwitchToGameplayCommand(const std::string& gameplaySceneName, BombermanGame& game)
	:Command::Command(),
	m_GameplaySceneName{ gameplaySceneName },
	m_BomberGame{ game }
{
}

void bombGame::SwitchToGameplayCommand::Execute(float)
{
	m_BomberGame.GetStateMachine().RequestStateTransition(std::make_unique<GameplayGameState>(m_BomberGame));
}
