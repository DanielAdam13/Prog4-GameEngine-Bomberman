#pragma once
#include "LevelGrid.h"

#include <vector>
#include <utility>

namespace ge
{
	class Scene;
	class SpriteSheet;
	class GameObject;
	class Texture2D;
}

namespace bombGame
{
	namespace stageLoader
	{
		struct EnemyEntry;
	}

	namespace levelBuilder
	{
		void BuildStaticGeometry(ge::Scene& scene, const LevelGrid& grid);

		// Generate and push the dynamic walls to the scene and store them in the grid as a reference
		void GenerateDynamicObjects(ge::Scene& scene, LevelGrid& grid,
			ge::SpriteSheet* breakableWallSheet, ge::Texture2D* exitDoorTexture,
			int breakableWallRandomnessIndex);

		void GenerateEnemies(ge::Scene& scene, LevelGrid& grid,
			const std::vector<stageLoader::EnemyEntry>& enemyEntries,
			const std::vector<ge::GameObject*>& players);

		// Helper for enemies
		std::vector<std::pair<int, int>> CollectEnemySpawnableTiles(const LevelGrid& grid, int minDistanceFromPlayers = 4);

	}
}