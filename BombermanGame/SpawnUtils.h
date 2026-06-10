#pragma once
#include "PowerupType.h"
#include "StageLoader.h"

#include <memory>
#include <glm/glm.hpp>
#include <functional>
#include <array>

namespace ge
{
	class GameObject;
	class Texture2D;
	class SpriteSheet;
	class Scene;
}

namespace bombGame
{
	class LevelGrid;
	struct EnemyArchetype;

	namespace spawnUtils
	{
		// Creates the reusable Bomb Game Object and returns it as a unique_ptr
		std::unique_ptr<ge::GameObject> CreateBomb(LevelGrid* grid, const glm::vec3& position, 
			ge::SpriteSheet* bombTexture, std::array<ge::SpriteSheet*, 3> explosionSheets, 
			float windupTimer, int explosionArmLength);

		void DetonateBombAt(LevelGrid& grid, ge::Scene& scene,
			const glm::vec3& bombCenter, int armLength, const std::array<ge::SpriteSheet*, 3>& explosionSheets,
			float lifetime);

		// Creates the Explosion Game Object on the correct collision Layer
		void CreateExplosionPart(ge::Scene& scene, const LevelGrid& grid,
			const glm::vec3& fixedPosition,	ge::SpriteSheet* texture, float lifetime, const std::vector<int>& animationFrames);

		ge::GameObject* SpawnEnemy(ge::Scene& scene, LevelGrid* grid, 
			const EnemyArchetype& archetype, const std::vector<ge::GameObject*>& targets,
			const glm::vec3& spawnPos);
		ge::GameObject* SpawnEnemy(ge::Scene& scene, LevelGrid* grid,
			const EnemyArchetype& archetype, const std::vector<ge::GameObject*>& targets,
			int gridCol, int gridRow);

		std::vector<ge::GameObject*> SpawnEnemiesAtExit(ge::Scene& scene, LevelGrid* grid,
			const std::vector<stageLoader::EnemyEntry>& enemyEntries,
			const std::vector<ge::GameObject*>& players);

		void SpawnBreakableWallAt(ge::Scene& scene, LevelGrid& grid, int col, int row, const ge::SpriteSheet* brWallSheet);
		void SpawnExitAt(ge::Scene& scene, LevelGrid& grid, int col, int row, ge::Texture2D* exitdoorTexture);
		void SpawnPowerupAt(ge::Scene& scene, LevelGrid& grid, int col, int row,
			 PowerupType type, ge::Texture2D* powerupTexture, int score);
	}
}