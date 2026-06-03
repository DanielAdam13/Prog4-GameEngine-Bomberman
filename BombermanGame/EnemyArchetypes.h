#pragma once
#include "EnemyType.h"

namespace ge
{
	class SpriteSheet;
}

namespace bombGame
{
	struct EnemyArchetype
	{
		ge::SpriteSheet* sheet{ nullptr };
		float speed{ 60.f };
		int health{ 1 };
		float detectionRadius{ 200.f };
		float smartness{ 0.f };
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