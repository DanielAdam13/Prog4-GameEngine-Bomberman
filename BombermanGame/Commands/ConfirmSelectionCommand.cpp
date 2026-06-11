#include "ConfirmSelectionCommand.h"
#include "GameStates/PlayerModeState.h"

bombGame::ConfirmSelectionCommand::ConfirmSelectionCommand(PlayerModeState& state)
	:Command::Command(),
	m_GameState{ state }
{
}

void bombGame::ConfirmSelectionCommand::Execute(float)
{
	m_GameState.ConfirmCurrentSelection();
}
