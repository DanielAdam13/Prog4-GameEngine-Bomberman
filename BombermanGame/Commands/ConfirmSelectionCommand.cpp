#include "ConfirmSelectionCommand.h"
#include "GameStates/GameState.h"

bombGame::ConfirmSelectionCommand::ConfirmSelectionCommand(GameState* state)
	:Command::Command(),
	m_GameState{ state }
{
}

void bombGame::ConfirmSelectionCommand::Execute(float)
{
	m_GameState->ConfirmCurrentSelection();
}
