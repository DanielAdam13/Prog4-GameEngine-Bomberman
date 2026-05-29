#pragma once

namespace bombGame
{
	// Enum -> int convesion happens automatically
	enum GameEventId
	{
		PLAYER_DIED = 0,
		PLAYER_LOST_HEALTH,
		PLAYER_SCORE_CHANGED,
		LAY_BOMB,
		EXPLODED_BOMB,
		GAME_OVER,
		ENEMY_DIED
	};
}