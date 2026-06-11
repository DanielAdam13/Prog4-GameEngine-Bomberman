#include "SwitchToTransitionStateCommand.h"

#include "BombermanGame.h"
#include "GameStates/GameStateMachine.h"
#include "GameStates/StageTransitionState.h"

#include <utility>
#include <memory>

bombGame::SwitchToTransitionCommand::SwitchToTransitionCommand(BombermanGame& game)
	:Command::Command(),
	m_BomberGame{ game }
{
}

void bombGame::SwitchToTransitionCommand::Execute(float)
{
	m_BomberGame.GetStateMachine().RequestStateTransition(
		std::make_unique<StageTransitionState>(m_BomberGame));
}
