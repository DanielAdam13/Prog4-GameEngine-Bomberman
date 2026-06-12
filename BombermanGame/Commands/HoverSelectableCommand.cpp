#include "HoverSelectableCommand.h"
#include "GameStates/GameState.h"

bombGame::HoverSelectableCommand::HoverSelectableCommand(ge::ISelectableNavigator* nav, std::pair<int, int> delta)
	:m_Navigator{ nav },
	m_Delta{ delta }
{
}

void bombGame::HoverSelectableCommand::Execute(float)
{
	m_Navigator->MoveHover(m_Delta);
}
