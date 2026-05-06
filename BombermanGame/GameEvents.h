#pragma once

namespace bombGame
{
	// Enum -> int convesion happens automatically
	enum EventId
	{
		PLAYER_DIED,
		PLAYER_LOST_HEALTH,
		PLAYER_SCORE_CHANGED,
		LAY_BOMB,
		EXPLODED_BOMB,
		GAME_OVER
	};
}