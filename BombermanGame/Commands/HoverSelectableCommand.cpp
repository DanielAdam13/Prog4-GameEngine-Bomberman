#include "HoverSelectableCommand.h"
#include "GameStates/GameState.h"

bombGame::HoverSelectableCommand::HoverSelectableCommand(GameState* state, std::pair<int, int> delta)
	:m_GameState{ state },
	m_Delta{ delta }
{
}

void bombGame::HoverSelectableCommand::Execute(float)
{
	m_GameState->MoveHover(m_Delta);
}
