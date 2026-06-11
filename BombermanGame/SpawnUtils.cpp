#include "SpawnUtils.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "Components/AnimatorComponent.h"
#include "Components/BombComponent.h"
#include "Components/ExplosionComponent.h"
#include "Scene.h"
#include "Components/Colliders.h"
#include "Components/BreakableWallComponent.h"
#include "Components/HealthComponent.h"
#include "Components/EnemyComponent.h"
#include "Components/PowerupComponent.h"
#include "Components/ScoreComponent.h" // player
#include "Components/PlayerComponent.h" // player
#include "Components/BombLayerComponent.h" // player
#include "SoundManager.h" // for player subjects

#include "LevelGrid.h"
#include "LevelLoader.h"
#include "EnemyArchetypes.h"
#include "Animation.h"
#include "SpriteSheet.h"

#include <utility>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <memory>

ge::GameObject* bombGame::spawnUtils::SpawnPlayerAt(ge::Scene& scene, LevelGrid& grid,
	std::pair<int, int> spawnLoc, ge::SpriteSheet* playerSheet,
	SoundManager* soundManager, 
	ge::SpriteSheet* bombSheet, std::array<ge::SpriteSheet*, 3> explosionSheets)
{
	auto tileSize{ grid.GetTileSize() };
	auto topBgPosition{ grid.GetLevelTopLeft() };

	auto player1GO = std::make_unique<ge::GameObject>("GO_Player");

	auto player1Animator{ player1GO->AddComponent<ge::AnimatorComponent>(player1GO.get(), playerSheet) };
	player1Animator->AddAnimation({ "idle", {3}, 1, false });
	player1Animator->AddAnimation({ "walk_left", {0, 1, 2}, 10, true });
	player1Animator->AddAnimation({ "walk_down", {3, 4, 5}, 10, true });
	player1Animator->AddAnimation({ "walk_right", {7, 8, 9}, 10, true });
	player1Animator->AddAnimation({ "walk_up", {10, 11, 12}, 10, true });
	player1Animator->AddAnimation({ "death", { 14, 15, 16, 17, 18, 19, 20}, 3, false });

	auto player1Tr{ player1GO->GetComponent<ge::Transform>() };
	const glm::vec3 player1Pos{
		topBgPosition.x + spawnLoc.first * tileSize,
		topBgPosition.y + spawnLoc.second * tileSize, 0.f };
	player1Tr->SetLocalPosition(player1Pos);
	const float playerScale{ tileSize / (playerSheet->GetFrameWidth() + 1) };
	player1Tr->SetLocalScale({ playerScale, playerScale, 1.f });

	auto player1BoxColl{ player1GO->AddComponent<ge::BoxCollider>(player1GO.get(),
		player1Animator->GetSingleFrameRectSize()) };
	player1BoxColl->AssignTag("Player");

	player1GO->AddComponent<ge::HealthComponent>(player1GO.get(), 1);
	player1GO->AddComponent<ge::ScoreComponent>(player1GO.get(), 0);
	auto player1PlayerComp{ player1GO->AddComponent<bombGame::PlayerComponent>(player1GO.get(), 150.f) };
	player1PlayerComp->GetDamageEvent().AddObserver(soundManager);
	player1PlayerComp->GetDeadEvent().AddObserver(soundManager);
	player1PlayerComp->GetScoreChangeEvent().AddObserver(soundManager);
	player1PlayerComp->GetMovedHorEvent().AddObserver(soundManager);
	player1PlayerComp->GetMovedVertEvent().AddObserver(soundManager);
	player1PlayerComp->GetPowerupPickedUpEvent().AddObserver(soundManager);

	auto player1BombLayer{ player1GO->AddComponent<bombGame::BombLayerComponent>(player1GO.get(), grid,
		bombSheet, explosionSheets,
		3.f, 1, 1) };
	player1BombLayer->GetLaidBombEvent().AddObserver(soundManager);
	player1BombLayer->GetBombExplodedEvent().AddObserver(soundManager);

	auto* playerGORaw{ player1GO.get() };
	scene.Add(std::move(player1GO));
	return playerGORaw;
}

std::unique_ptr<ge::GameObject> bombGame::spawnUtils::CreateBomb(LevelGrid& grid, const glm::vec3& position,
	ge::SpriteSheet* bombSheet, std::array<ge::SpriteSheet*, 3> explosionSheets, 
	float windupDuration, int explosionArmLength)
{
	auto bomb = std::make_unique<ge::GameObject>("GO_Bomb");
	auto* bombTr{ bomb->GetComponent<ge::Transform>() };
	bombTr->SetLocalPosition(position);
	bombTr->SetLocalScale(2.5f, 2.5f, 1.f);
	
	auto* animator{ bomb->AddComponent<ge::AnimatorComponent>(bomb.get(), bombSheet) };
	animator->SetAnchor({ 0.5f, 0.5f });
	animator->AddAnimation({ "wind_up", {0, 1, 2}, 4, true });
	animator->Play("wind_up");

	bomb->AddComponent<BombComponent>(bomb.get(), grid, windupDuration, explosionArmLength, explosionSheets);

	return bomb;
}

void bombGame::spawnUtils::DetonateBombAt(LevelGrid& grid, ge::Scene& scene,
	const glm::vec3& bombDropCenter, int armLength, const std::array<ge::SpriteSheet*, 3>& explosionSheets, float lifetime)
{
	auto bombTile{ grid.GetGridTileAt(bombDropCenter) };
	if (!bombTile)
		return;

	const int bombDroppedTileCol{ bombTile->col };
	const int bombDroppedTileRow{ bombTile->row };

	// Middle part
	CreateExplosionPart(scene, grid, bombDropCenter, explosionSheets[0], lifetime, { 0,1,2,3 });

	// Explosion arms creation logic
	constexpr std::pair<int, int> directions[4]{ {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
	for (auto [dx, dy] : directions)
	{
		for (int step{ 1 }; step <= armLength; ++step)
		{
			const int currentCol{ bombDroppedTileCol + dx * step };
			const int currentRow{ bombDroppedTileRow + dy * step };

			auto tile{ grid.GetGridTileByCoord(currentCol, currentRow) };
			if (!tile)
				break;

			// If Static wall - arm is not spawned
			if (tile->gridTileType == levelLoader::TileType::Wall)
				break;

			// Chain detonation: if there's a bomb here, detonate and stop it
			if (auto* bombGO = grid.GetBombAt(currentCol, currentRow)) 
			{
				if (auto* bc = bombGO->GetComponent<BombComponent>()) 
				{
					bc->ForceDetonate();
				}
				break;   // arm stops at the chained bomb
			}

			// If Breakable Wall is not nullptr - CRUMBLE wall and arm is not spawned
			if (auto* breakableWall = grid.GetBreakableWallAt(currentCol, currentRow))
			{
				if (auto* brWallComp = breakableWall->GetComponent<BreakableWallComponent>())
				{
					brWallComp->Crumble();
					grid.ClearBreakableAt(currentCol, currentRow);
				}
				break;
			}

			// Else, when tile is empty - spawn explosion arm
			const auto sheet{ (dx == 0) ? 
				explosionSheets[1] : explosionSheets[2] };

			const std::vector<int> animationFrames{ (dx == -1 || dy == -1) ? 
				std::vector<int>{0, 1, 2, 3} : std::vector<int>{4, 5, 6, 7} };

			const auto gridRectSize{ tile->gridRect.size };
			const auto gridRectPos{ tile->gridRect.position + gridRectSize / 2.f };
			CreateExplosionPart(scene, grid, {gridRectPos.x, gridRectPos.y, 0.f}, sheet, lifetime, animationFrames);
		}
	}
}

void bombGame::spawnUtils::CreateExplosionPart(ge::Scene& scene, const LevelGrid&,
	const glm::vec3& fixedPosition, ge::SpriteSheet* explosionSheet, float activeTimer, const std::vector<int>& animationFrames)
{
	auto explosion = std::make_unique<ge::GameObject>("GO_ExplosionArm");
	auto* explosionTr{ explosion->GetComponent<ge::Transform>() };
	explosionTr->SetLocalPosition(fixedPosition);
	explosionTr->SetLocalScale(2.5f, 2.5f, 1.f);

	auto* animator{ explosion->AddComponent<ge::AnimatorComponent>(explosion.get(), explosionSheet)};
	animator->SetAnchor({ 0.5f, 0.5f });
	animator->AddAnimation({ "explode", animationFrames, 10, false });
	animator->Play("explode");

	explosion->AddComponent<ExplosionComponent>(explosion.get(), activeTimer);

	/*explosion->AddComponent<ge::BoxCollider>(explosion.get(), glm::vec2{ grid.GetTileSize(), grid.GetTileSize() },
		true, glm::vec2{ -grid.GetTileSize() / 2, -grid.GetTileSize() / 2})
		->AssignTag("Explosion");*/

	explosion->AddComponent<ge::BoxCollider>(explosion.get(), animator->GetSingleFrameRectSize(),
		true, glm::vec2{ -explosionSheet->GetFrameWidth() / 2, -explosionSheet->GetFrameHeight() / 2 })
		->AssignTag("Explosion");

	scene.Add(std::move(explosion));
}

ge::GameObject* bombGame::spawnUtils::SpawnEnemy(ge::Scene& scene, LevelGrid* grid,
	const EnemyArchetype& archetype, const std::vector<ge::GameObject*>& targets,
	const glm::vec3& spawnPos, bool withoutAI)
{
	auto enemyGO = std::make_unique<ge::GameObject>("GO_IceEnemy1");

	auto* enemy1Animator{ enemyGO->AddComponent<ge::AnimatorComponent>(enemyGO.get(), archetype.sheet) };
	enemy1Animator->AddAnimation({ "walk_right", {0, 1, 2}, 4, true });
	enemy1Animator->AddAnimation({ "walk_left", {3, 4, 5}, 4, true });
	enemy1Animator->AddAnimation({ "death", {6, 7, 8, 9, 10}, 3, false });
	enemy1Animator->AddAnimation({ "idle", {0}, 1, false });
	enemy1Animator->SetAnchor({ 0.5f, 0.5f });

	auto enemy1Tr{ enemyGO->GetComponent<ge::Transform>() };
	enemy1Tr->SetLocalPosition(spawnPos);
	enemy1Tr->SetLocalScale({ 2.7f, 2.7f, 1.f });

	const float gridTileSize{ grid->GetTileSize() };
	auto enemy1BoxColl{ enemyGO->AddComponent<ge::BoxCollider>(enemyGO.get(),
		glm::vec2{gridTileSize, gridTileSize}, true,
		glm::vec2{-gridTileSize * 0.5f , -gridTileSize * 0.5f})};
	enemy1BoxColl->AssignTag("Enemy");

	enemyGO->AddComponent<ge::HealthComponent>(enemyGO.get(), archetype.health);

	auto enemyComp{ enemyGO->AddComponent<EnemyComponent>(enemyGO.get(), grid,
		archetype.speed, archetype.detectionRadius, archetype.scoreValue) };

	if (!withoutAI)
	{
		for (auto t : targets)
		{
			if (t && !t->MarkedForDeletion())
			{
				enemyComp->AddTarget(t);
			}
		}
		enemyComp->InitializeStates();
	}

	auto* enemyRaw{ enemyGO.get() };
	scene.Add(std::move(enemyGO));

	return enemyRaw;
}

ge::GameObject* bombGame::spawnUtils::SpawnEnemy(ge::Scene& scene, LevelGrid* grid, 
	const EnemyArchetype& archetype, const std::vector<ge::GameObject*>& targets,
	int gridCol, int gridRow, bool withoutAI)
{
	const auto gridTileMid{ grid->GetMidGridTilePointByCoord(gridCol, gridRow) };
	const glm::vec3 spawnPos{ gridTileMid->x, gridTileMid->y, 0.f };
	return SpawnEnemy(scene, grid, archetype, targets, spawnPos, withoutAI);
}

std::vector<ge::GameObject*> bombGame::spawnUtils::SpawnEnemiesAtExit(ge::Scene& scene, LevelGrid* grid, 
	const std::vector<stageLoader::EnemyEntry>& enemyEntries, const std::vector<ge::GameObject*>& players)
{
	std::vector<ge::GameObject*> spawnedEnemies;

	const auto [col, row] = grid->GetExitLocation();

	for (auto& enemyEntry : enemyEntries)
	{
		for (int i{}; i < enemyEntry.count; ++i)
		{
			const auto& arch{ enemyArchetypes::Get(enemyEntry.type) };

			auto enemyGo{ SpawnEnemy(scene, grid, arch, players, col, row) };
			spawnedEnemies.push_back(enemyGo);
		}
	}

	return spawnedEnemies;
}

void bombGame::spawnUtils::SpawnBreakableWallAt(ge::Scene& scene, LevelGrid& grid, int col, int row, const ge::SpriteSheet* brWallSheet)
{
	const int currentIndex{ grid.ToIndex(col, row) };
	auto breakableWallGO = std::make_unique<ge::GameObject>("GO_BreakableWall" + std::to_string(currentIndex));
	const glm::vec3 brWallPos{ col * grid.GetTileSize(), row * grid.GetTileSize(), 0.f };
	auto breakTr{ breakableWallGO->GetComponent<ge::Transform>() };
	breakTr->SetLocalPosition(grid.GetLevelTopLeft() + brWallPos);
	breakTr->SetLocalScale(3.5f, 3.5f, 1.f);

	auto* brWallAnimator{ breakableWallGO->AddComponent<ge::AnimatorComponent>(breakableWallGO.get(), brWallSheet) };
	brWallAnimator->AddAnimation({ "static", {0}, 1, false });
	brWallAnimator->AddAnimation({ "crumbling", {0, 1, 2, 3, 4, 5, 6}, 20, false });
	brWallAnimator->Play("static");

	breakableWallGO->AddComponent<BreakableWallComponent>(breakableWallGO.get(), 0.4f);

	auto* collider{ breakableWallGO->AddComponent<ge::BoxCollider>(breakableWallGO.get(),
		glm::vec2{ grid.GetTileSize(), grid.GetTileSize() }, true) };
	collider->AssignTag("BreakableWall");

	grid.RegisterBreakableAt(col, row, breakableWallGO.get());

	scene.Add(std::move(breakableWallGO));
}

void bombGame::spawnUtils::SpawnExitAt(ge::Scene& scene, LevelGrid& grid, int col, int row, ge::Texture2D* exitDoorTexture)
{
	auto exitGO = std::make_unique<ge::GameObject>("GO_ExitDoor");
	const glm::vec3 exitPos{ col * grid.GetTileSize(), row * grid.GetTileSize(), 0.f };
	auto exitTr{ exitGO->GetComponent<ge::Transform>() };
	exitTr->SetLocalPosition(grid.GetLevelTopLeft() + exitPos);
	exitTr->SetLocalScale(3.6f, 3.6f, 1.f);

	auto* exitImage{ exitGO->AddComponent<ge::Image>(exitGO.get()) };
	exitImage->SetTexture(exitDoorTexture);

	scene.Add(std::move(exitGO));

	grid.MarkExitLocationAt(col, row);
}

void bombGame::spawnUtils::SpawnPowerupAt(ge::Scene& scene, LevelGrid& grid, int col, int row, 
	PowerupType type, ge::Texture2D* powerupTexture, int score)
{
	auto powerupGO = std::make_unique<ge::GameObject>("GO_Powerup");
	const glm::vec3 powerupPos{ col * grid.GetTileSize(), row * grid.GetTileSize(), 0.f };
	auto powerTr{ powerupGO->GetComponent<ge::Transform>() };
	powerTr->SetLocalPosition(grid.GetLevelTopLeft() + powerupPos);
	powerTr->SetLocalScale(3.6f, 3.6f, 1.f);

	auto* powerupImage{ powerupGO->AddComponent<ge::Image>(powerupGO.get()) };
	powerupImage->SetTexture(powerupTexture);
	
	// Box Collider
	auto* collider{ powerupGO->AddComponent<ge::BoxCollider>(powerupGO.get(),
		glm::vec2{ grid.GetTileSize(), grid.GetTileSize() }, true) };
	collider->AssignTag("Powerup");

	// Powerup Component
	powerupGO->AddComponent<PowerupComponent>(powerupGO.get(), type, score);

	scene.Add(std::move(powerupGO));
}
