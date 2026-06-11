#pragma once
#include "PowerupType.h"

#include <vector>

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	struct GameSession;

	// Helper. Decoupled from PowerupComponent and PlayerComponent 
	// since this logic interacts with GameSession and Gameplay directly.
	namespace powerupEffects
	{
		// Helper called from Gameplay State's Notify. Calls ApplyEffectToPlayer() and writes to game session.
		void OnPickup(PowerupType type, const std::vector<ge::GameObject*>& players, GameSession& session);

		// !! Actual powerup effect applying to the player !!
		void ApplyEffectToPlayer(ge::GameObject* player, PowerupType type);

		// Calls ApplyEffectToPlayer multiple times
		void ApplyStoredToPlayer(ge::GameObject* player, const std::vector<PowerupType>& stored);
	}
}