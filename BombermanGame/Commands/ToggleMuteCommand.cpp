#include "ToggleMuteCommand.h"
#include "BombermanGame.h"

bombGame::ToggleMuteCommand::ToggleMuteCommand(BombermanGame& game)
	:Command::Command(),
	m_Game{ game }
{
}

void bombGame::ToggleMuteCommand::Execute(float)
{
	m_Game.GetSoundManager().SetMuted(!m_Game.GetSoundManager().IsMuted());
}
