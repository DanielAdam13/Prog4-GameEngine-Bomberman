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

bombGame::GameSession& bombGame::GameState::GetCachedGameSession() noexcept
{
	return m_Game.GetCurrentGameSession();
}
