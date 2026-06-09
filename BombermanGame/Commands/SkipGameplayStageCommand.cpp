#include "SkipGameplayStageCommand.h"
#include "BombermanGame.h"
#include "GameStates/GameStateMachine.h"
#include "GameStates/GameplayState.h"
#include "GameStates/VictoryState.h"
#include "StageLoader.h"

#include <memory>

bombGame::SkipGameplayStageCommand::SkipGameplayStageCommand(BombermanGame& game)
	:Command::Command(),
	m_BomberGame{ game }
{
}

void bombGame::SkipGameplayStageCommand::Execute(float)
{
	m_BomberGame.AdvanceGameplayStageIndex();
	
	if (m_BomberGame.GetCurrentGameSession().currentStageIndex >= stageLoader::GetStageCount())
	{
		m_BomberGame.GetStateMachine().RequestStateTransition(std::make_unique<VictoryState>(m_BomberGame));
	}
	else
	{
		m_BomberGame.GetStateMachine().RequestStateTransition(std::make_unique<GameplayGameState>(m_BomberGame));
	}
}