#include "ConfirmSelectionCommand.h"
#include "GameStates/GameState.h"

bombGame::ConfirmSelectionCommand::ConfirmSelectionCommand(ge::ISelectableNavigator* nav)
	:Command::Command(),
	m_Navigator{ nav }
{
}

void bombGame::ConfirmSelectionCommand::Execute(float)
{
	m_Navigator->ConfirmCurrentSelection();
}
