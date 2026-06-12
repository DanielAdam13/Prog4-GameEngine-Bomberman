#include "SwitchToMainMenuCommand.h"

#include "BombermanGame.h"
#include "GameStates/GameStateMachine.h"
#include "GameStates/MainMenuState.h"

#include <memory>

bombGame::SwitchToMainMenuCommand::SwitchToMainMenuCommand(BombermanGame& game)
	:Command::Command(),
	m_BomberGame{ game }
{
}

void bombGame::SwitchToMainMenuCommand::Execute(float)
{
	m_BomberGame.GetStateMachine().RequestStateTransition(
		std::make_unique<MainMenuGameState>(m_BomberGame));
}
