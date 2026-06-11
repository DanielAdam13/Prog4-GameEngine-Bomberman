#include "LevelBuilder.h"

#include "LevelGrid.h"
#include "StageLoader.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Colliders.h"
#include "Scene.h"
#include "Structs.h"

#include "SpriteSheet.h"
#include "SpawnUtils.h"
#include "EnemyArchetypes.h"
#include "PowerupArchetypes.h"

#include <utility>
#include <memory>
#include <random>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

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
	ge::SpriteSheet* breakableWallSheet, ge::Texture2D* exitDoorTexture, const stageLoader::PowerupEntry& stagePowerup,
	int breakableWallRandomnessIndex)
{
	static std::mt19937 gen{ std::random_device{}() };

	const auto& layout{ grid.GetLevelLayout() };

	// 1. Collect empty tiles
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

	// Shuffle to get random order; then we consume from the front
	std::shuffle(emptyTiles.begin(), emptyTiles.end(), gen);
	std::vector<std::pair<int, int>> reservedTiles;

	// 2. Pick exit tile
	const auto [exitCol, exitRow] { emptyTiles[0]};
	spawnUtils::SpawnExitAt(scene, grid, exitCol, exitRow, exitDoorTexture); // Saves exit coordinate to grid internally
	spawnUtils::SpawnBreakableWallAt(scene, grid, exitCol, exitRow, breakableWallSheet);
	// !!
	reservedTiles.emplace_back(exitCol, exitRow);

	// 3. Pick Powerup tiles
	const auto& arch{ powerupArchetypes::Get(stagePowerup.type) };
	size_t powerupTileIdx{ 1 };

	for (int i{}; i < stagePowerup.count; ++i) 
	{
		if (powerupTileIdx >= emptyTiles.size()) // out of room
			break;   

		const auto [pCol, pRow] = emptyTiles[powerupTileIdx++];
		spawnUtils::SpawnPowerupAt(scene, grid, pCol, pRow,
			stagePowerup.type, arch.texture, arch.scoreValuePicked);
		spawnUtils::SpawnBreakableWallAt(scene, grid, pCol, pRow, breakableWallSheet);
		// !!
		reservedTiles.emplace_back(pCol, pRow);
	}

	// 4. Spawn random breakable walls
	static std::uniform_int_distribution<size_t> dist(0, breakableWallRandomnessIndex);

	auto isReserved{ [&reservedTiles](int col, int row) -> bool 
		{
			for (const auto& [rc, rr] : reservedTiles)
			{
				if (rc == col && rr == row)
					return true;
			}
			return false; 
		} };

	for (int row{ 0 }; row < layout.height; ++row)
	{
		for (int col{ 0 }; col < layout.width; ++col)
		{
			// Skip exit and powerup tiles
			if (isReserved(col, row))
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

std::vector<ge::GameObject*> bombGame::levelBuilder::GenerateEnemies(ge::Scene& scene, LevelGrid& grid,
	const std::vector<stageLoader::EnemyEntry>& enemyEntries, 
	const std::vector<ge::GameObject*>& players)
{
	std::vector<ge::GameObject*> spawnedEnemies;

	auto eligibleTiles{ CollectEnemySpawnableTiles(grid) };

	static std::mt19937 gen{ std::random_device{}() };
	std::shuffle(eligibleTiles.begin(), eligibleTiles.end(), gen);

	size_t tileIdx{ 0 };
	for (auto& enemyEntry : enemyEntries)
	{
		for (int i{}; i < enemyEntry.count; ++i)
		{
			if (tileIdx >= eligibleTiles.size()) // Out of room for spawning(unlikely)
				return spawnedEnemies; // return spawned enemies so far

			const auto [col, row] = eligibleTiles[tileIdx++];

			const auto& arch{ enemyArchetypes::Get(enemyEntry.type) };
			
			auto enemyGo{ spawnUtils::SpawnEnemy(scene, &grid, arch, players, col, row) };
			spawnedEnemies.push_back(enemyGo);
		}
	}

	return spawnedEnemies;
}

std::vector<std::pair<int, int>> bombGame::levelBuilder::CollectEnemySpawnableTiles(const LevelGrid& grid, int minDistanceFromPlayers)
{
	std::vector<std::pair<int, int>> eligible;
	const auto& layout{ grid.GetLevelLayout() };

	const std::pair<int, int> playerCoords[2] = {
		{ layout.player1SpawnPoint.first, layout.player1SpawnPoint.second },
		{ layout.player2SpawnPoint.first, layout.player2SpawnPoint.second },
	};

	for (int row{}; row < layout.height; ++row) 
	{
		for (int col{}; col < layout.width; ++col) 
		{
			// 1. No wall or player spawn tile
			if (layout.At(col, row) != levelLoader::TileType::Empty) 
				continue;
			// 2. No Breakable Wall at tile
			if (grid.GetBreakableWallAt(col, row)) 
				continue;

			// 3. Far enough from players
			bool tooClose = false;
			for (const auto& [pc, pr] : playerCoords) 
			{
				if (pc < 0 || pr < 0) // Skip invalid players - for single mode vs coop
					continue;

				const int manhattan{ std::abs(col - pc) + std::abs(row - pr) };

				if (manhattan < minDistanceFromPlayers) 
				{
					tooClose = true;
					break;
				}
			}
			if (tooClose) 
				continue;

			eligible.emplace_back(col, row);
		}
	}

	return eligible;
}
