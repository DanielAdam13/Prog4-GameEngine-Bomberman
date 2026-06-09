#pragma once
#include "EnemyType.h"

namespace ge
{
	class SpriteSheet;
}

namespace bombGame
{
	// Type Object - TYPED Object is EnemyComponent
	struct EnemyArchetype
	{
		ge::SpriteSheet* sheet{ nullptr };
		float speed{ 60.f };
		int health{ 1 };
		float detectionRadius{ 200.f };
		int scoreValue{ 100 };
	};

	namespace enemyArchetypes
	{
		const EnemyArchetype& Get(EnemyType type);
		void InitializeArchetypes(ge::SpriteSheet* balloomSheet,
			ge::SpriteSheet* onilSheet,
			ge::SpriteSheet* dallSheet,
			ge::SpriteSheet* minvoSheet);
	}
}