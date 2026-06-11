#include "HoverSelectableCommand.h"
#include "GameStates/PlayerModeState.h"

bombGame::HoverSelectableCommand::HoverSelectableCommand(PlayerModeState& state, int delta)
	:m_GameState{ state },
	m_Delta{ delta }
{
}

void bombGame::HoverSelectableCommand::Execute(float)
{
	m_GameState.MoveHover(m_Delta);
}
