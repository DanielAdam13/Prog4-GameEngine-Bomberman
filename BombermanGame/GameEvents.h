#pragma once

namespace bombGame
{
	namespace events
	{
		// Enum -> int convesion happens automatically
		// AT least it does not pollute the whole bombGame namespace
		enum GameEventId
		{
			PLAYER_DIED = 0,
			PLAYER_LOST_HEALTH,
			PLAYER_SCORE_CHANGED,
			LAY_BOMB,
			EXPLODED_BOMB,
			GAME_OVER,
			ENEMY_DIED,
			PLAYER_MOVED_HOR,
			PLAYER_MOVED_VERT,
			POWERUP_PICKED_UP
		};
	}
}