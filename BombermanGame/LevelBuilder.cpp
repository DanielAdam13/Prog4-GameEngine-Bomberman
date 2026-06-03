#include "LevelBuilder.h"

#include "LevelGrid.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Colliders.h"
#include "Scene.h"
#include "Structs.h"
#include "Components/BreakableWallComponent.h"

#include "Components/AnimatorComponent.h"
#include "SpriteSheet.h"
#include "Animation.h"
#include "SpawnUtils.h"

#include <utility>
#include <memory>
#include <random>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

void bombGame::levelBuilder::BuildStaticGeometry(ge::Scene& scene, const LevelGrid& grid)
{
	for (int row{ 0 }; row < grid.GetLevelLayout().height; ++row)
	{
		for (int col{ 0 }; col < grid.GetLevelLayout().width; ++col)
		{
			levelLoader::TileType currentTileType{ grid.GetLevelLayout().At(col, row)};
			if (currentTileType == levelLoader::TileType::Wall)
			{
				const int currentIndex{ grid.ToIndex(col, row) };
				auto wallGameObject = std::make_unique<ge::GameObject>("GO_Wall" + std::to_string(currentIndex));
				const glm::vec3 wallPos{ col * grid.GetTileSize() , row * grid.GetTileSize(), 0.f};
				wallGameObject->GetComponent<ge::Transform>()->SetLocalPosition(grid.GetLevelTopLeft() + wallPos);
				auto collider{ wallGameObject->AddComponent<ge::BoxCollider>(wallGameObject.get(),
					glm::vec2{ grid.GetTileSize(), grid.GetTileSize() }, false) };
				collider->AssignTag("Wall");

				scene.Add(std::move(wallGameObject));
			}
		}
	}
}

void bombGame::levelBuilder::GenerateDynamicObjects(ge::Scene& scene, LevelGrid& grid, 
	ge::SpriteSheet* breakableWallSheet, ge::Texture2D* exitDoorTexture, int breakableWallRandomnessIndex)
{
	static std::mt19937 gen{ std::random_device{}() };

	const auto& layout{ grid.GetLevelLayout() };

	// 1. Pick Exit tile
	std::vector<std::pair<int, int>> emptyTiles;
	for (int row{ 0 }; row < layout.height; ++row)
	{
		for (int col{ 0 }; col < layout.width; ++col)
		{
			if (layout.At(col, row) == levelLoader::TileType::Empty)
			{
				emptyTiles.emplace_back(col, row);
			}
		}
	}
	if (emptyTiles.empty()) // Invalid level
		return;

	std::uniform_int_distribution<size_t> pick(0, emptyTiles.size() - 1);
	const auto [exitCol, exitRow] { emptyTiles[pick(gen)]};

	spawnUtils::SpawnExitAt(scene, grid, exitCol, exitRow, exitDoorTexture); // Saves exit coordinate to grid internally
	spawnUtils::SpawnBreakableWallAt(scene, grid, exitCol, exitRow, breakableWallSheet);

	// 2. Spawn random breakable walls
	static std::uniform_int_distribution<size_t> dist(0, breakableWallRandomnessIndex);
	for (int row{ 0 }; row < layout.height; ++row)
	{
		for (int col{ 0 }; col < layout.width; ++col)
		{
			// Skip exit
			if (row == exitRow && col == exitCol)
				continue;

			levelLoader::TileType currentTileType{ layout.At(col, row) };

			// 1 out of [breakableWallRandomnessIndex] chance to spawn a breakable wall on an Empty spot
			if (currentTileType == levelLoader::TileType::Empty)
			{
				if (dist(gen) == 0)
				{
					spawnUtils::SpawnBreakableWallAt(scene, grid, col, row, breakableWallSheet);
				}
			}
		}
	}
}
