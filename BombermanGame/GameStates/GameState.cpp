#include "GameState.h"
#include "BombermanGame.h"

bombGame::GameState::GameState(BombermanGame& game)
	:m_Game{ game }
{
}

bombGame::BombermanGame& bombGame::GameState::GetBombermanGame() noexcept
{
	return m_Game;
}

const bombGame::GameSession& bombGame::GameState::GetCachedGameSession() const noexcept
{
	return m_Game.GetCurrentGameSession();
}
