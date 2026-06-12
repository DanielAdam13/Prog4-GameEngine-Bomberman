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
	ge::IObserver* soundObserver, ge::IObserver* powerupObserver,
	ge::SpriteSheet* bombSheet, std::array<ge::SpriteSheet*, 3> explosionSheets,
	int startingScore)
{
	auto tileSize{ grid.GetTileSize() };

	auto playerGO = std::make_unique<ge::GameObject>("GO_Player");

	auto playerAnimator{ playerGO->AddComponent<ge::AnimatorComponent>(playerGO.get(), playerSheet) };
	playerAnimator->AddAnimation(ge::Animation{ "idle", {3}, 1, false });
	playerAnimator->AddAnimation({ "walk_left", {0, 1, 2}, 10, true });
	playerAnimator->AddAnimation({ "walk_down", {3, 4, 5}, 10, true });
	playerAnimator->AddAnimation({ "walk_right", {7, 8, 9}, 10, true });
	playerAnimator->AddAnimation({ "walk_up", {10, 11, 12}, 10, true });
	playerAnimator->AddAnimation({ "death", { 14, 15, 16, 17, 18, 19, 20}, 3, false });
	playerAnimator->SetAnchor({ 0.5f, 0.5f });

	auto playerPos{ grid.GetMidGridTilePointByCoord(spawnLoc.first, spawnLoc.second) };
	auto playerTr{ playerGO->GetComponent<ge::Transform>() };
	playerTr->SetLocalPosition({ playerPos->x, playerPos->y, 0.f });
	const float playerScale{ tileSize / (playerSheet->GetFrameWidth() + 1) };
	playerTr->SetLocalScale({ playerScale, playerScale, 1.f });

	const float playerCollSize{ tileSize * 0.95f };
	auto playerBoxColl{ playerGO->AddComponent<ge::BoxCollider>(playerGO.get(),
		glm::vec2{playerCollSize, playerCollSize}, true,
		glm::vec2{-playerCollSize * 0.5f , -playerCollSize * 0.5f}) };
	playerBoxColl->AssignTag("Player");

	playerGO->AddComponent<ge::HealthComponent>(playerGO.get(), 1);
	playerGO->AddComponent<ge::ScoreComponent>(playerGO.get(), startingScore);
	auto playerPlayerComp{ playerGO->AddComponent<bombGame::PlayerComponent>(playerGO.get(), 150.f, true) };
	playerPlayerComp->GetDamageEvent().AddObserver(soundObserver);
	playerPlayerComp->GetDeadEvent().AddObserver(soundObserver);
	playerPlayerComp->GetScoreChangeEvent().AddObserver(soundObserver);
	playerPlayerComp->GetMovedHorEvent().AddObserver(soundObserver);
	playerPlayerComp->GetMovedVertEvent().AddObserver(soundObserver);
	// Two observers for powerup pickup event
	playerPlayerComp->GetPowerupPickedUpEvent().AddObserver(soundObserver);
	playerPlayerComp->GetPowerupPickedUpEvent().AddObserver(powerupObserver);

	auto playerBombLayer{ playerGO->AddComponent<bombGame::BombLayerComponent>(playerGO.get(), grid,
		bombSheet, explosionSheets,
		3.f, 1, 1) };
	playerBombLayer->GetLaidBombEvent().AddObserver(soundObserver);
	playerBombLayer->GetBombExplodedEvent().AddObserver(soundObserver);

	auto* playerGORaw{ playerGO.get() };
	scene.Add(std::move(playerGO));
	return playerGORaw;
}

ge::GameObject* bombGame::spawnUtils::SpawnEnemyPlayerAt(ge::Scene& scene, LevelGrid& grid, 
	std::pair<int, int> spawnLoc, ge::SpriteSheet* playerSheet,
	ge::IObserver* soundObserver, ge::IObserver* powerupObserver)
{
	auto tileSize{ grid.GetTileSize() };

	auto enemyPlayerGO = std::make_unique<ge::GameObject>("GO_EnemyPlayer");

	auto anim{ enemyPlayerGO->AddComponent<ge::AnimatorComponent>(enemyPlayerGO.get(), playerSheet) };
	anim->AddAnimation({ "idle", {0}, 1, false });
	anim->AddAnimation({ "walk_left", {3, 4, 5}, 4, true });
	anim->AddAnimation({ "walk_down", {3, 4, 5}, 4, true });
	anim->AddAnimation({ "walk_right", {0, 1, 2}, 4, true });
	anim->AddAnimation({ "walk_up", {0, 1, 2}, 4, true });
	anim->AddAnimation({ "death", {6, 7, 8, 9, 10}, 3, false });
	anim->SetAnchor({ 0.5f, 0.5f });

	auto pos{ grid.GetMidGridTilePointByCoord(spawnLoc.first, spawnLoc.second) };
	auto tr{ enemyPlayerGO->GetComponent<ge::Transform>() };
	tr->SetLocalPosition({ pos->x, pos->y, 0.f });
	tr->SetLocalScale({ 3.f, 3.f, 1.f });

	const float collSize{ tileSize * 0.95f };
	auto player1BoxColl{ enemyPlayerGO->AddComponent<ge::BoxCollider>(enemyPlayerGO.get(),
		glm::vec2{collSize, collSize}, true,
		glm::vec2{-collSize * 0.5f , -collSize * 0.5f}) };
	player1BoxColl->AssignTag("Enemy");

	enemyPlayerGO->AddComponent<ge::HealthComponent>(enemyPlayerGO.get(), 1);
	enemyPlayerGO->AddComponent<ge::ScoreComponent>(enemyPlayerGO.get(), 0);
	auto plComp{ enemyPlayerGO->AddComponent<bombGame::PlayerComponent>(enemyPlayerGO.get(), 130.f, false) };
	plComp->GetDamageEvent().AddObserver(soundObserver);
	plComp->GetDeadEvent().AddObserver(soundObserver);
	plComp->GetScoreChangeEvent().AddObserver(soundObserver);
	plComp->GetMovedHorEvent().AddObserver(soundObserver);
	plComp->GetMovedVertEvent().AddObserver(soundObserver);
	// Two observers for powerup pickup event
	plComp->GetPowerupPickedUpEvent().AddObserver(soundObserver);
	plComp->GetPowerupPickedUpEvent().AddObserver(powerupObserver);

	// NO layer

	auto* enemyPlayerGORaw{ enemyPlayerGO.get() };
	scene.Add(std::move(enemyPlayerGO));
	return enemyPlayerGORaw;
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

	explosion->AddComponent<ge::BoxCollider>(explosion.get(), animator->GetSingleFrameRectSize(),
		true, glm::vec2{ -explosionSheet->GetFrameWidth() / 2, -explosionSheet->GetFrameHeight() / 2 })
		->AssignTag("Explosion");

	scene.Add(std::move(explosion));
}

ge::GameObject* bombGame::spawnUtils::SpawnEnemy(ge::Scene& scene, LevelGrid* grid,
	const EnemyArchetype& archetype, const std::vector<ge::GameObject*>& targets,
	const glm::vec3& spawnPos)
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
	enemy1Tr->SetLocalScale({ 3.f, 3.f, 1.f });

	const float gridTileSize{ grid->GetTileSize() };
	auto enemy1BoxColl{ enemyGO->AddComponent<ge::BoxCollider>(enemyGO.get(),
		glm::vec2{gridTileSize, gridTileSize}, true,
		glm::vec2{-gridTileSize * 0.5f , -gridTileSize * 0.5f})};
	enemy1BoxColl->AssignTag("Enemy");

	enemyGO->AddComponent<ge::HealthComponent>(enemyGO.get(), archetype.health);

	auto enemyComp{ enemyGO->AddComponent<EnemyComponent>(enemyGO.get(), grid,
		archetype.speed, archetype.detectionRadius, archetype.scoreValue) };

	for (auto t : targets)
	{
		if (t && !t->MarkedForDeletion())
		{
			enemyComp->AddTarget(t);
		}
	}
	enemyComp->InitializeStates();

	auto* enemyRaw{ enemyGO.get() };
	scene.Add(std::move(enemyGO));

	return enemyRaw;
}

ge::GameObject* bombGame::spawnUtils::SpawnEnemy(ge::Scene& scene, LevelGrid* grid, 
	const EnemyArchetype& archetype, const std::vector<ge::GameObject*>& targets,
	int gridCol, int gridRow)
{
	const auto gridTileMid{ grid->GetMidGridTilePointByCoord(gridCol, gridRow) };
	const glm::vec3 spawnPos{ gridTileMid->x, gridTileMid->y, 0.f };
	return SpawnEnemy(scene, grid, archetype, targets, spawnPos);
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
