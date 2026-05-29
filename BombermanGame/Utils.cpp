#include "Utils.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "Components/AnimatorComponent.h"
#include "Components/BombComponent.h"
#include "Components/ExplosionComponent.h"
#include "Scene.h"
#include "Components/Colliders.h"
#include "Components/BreakableWallComponent.h"

#include "LevelBuilder.h"
#include "LevelLoader.h"
#include "Animation.h"
#include "SpriteSheet.h"

#include <utility>
#include <array>
#include <vector>

std::unique_ptr<ge::GameObject> bombGame::spawnUtils::CreateBomb(LevelGrid* grid, const glm::vec3& position,
	ge::SpriteSheet* bombSheet, std::array<ge::SpriteSheet*, 3> explosionSheets, float windupDuration)
{
	auto bomb = std::make_unique<ge::GameObject>("GO_Bomb");
	auto* bombTr{ bomb->GetComponent<ge::Transform>() };
	bombTr->SetLocalPosition(position);
	bombTr->SetLocalScale(2.f, 2.f, 1.f);
	
	auto* animator{ bomb->AddComponent<ge::AnimatorComponent>(bomb.get(), bombSheet) };
	animator->SetAnchor({ 0.5f, 0.5f });
	animator->AddAnimation({ "wind_up", {0, 1, 2}, 4, true });
	animator->Play("wind_up");

	bomb->AddComponent<BombComponent>(bomb.get(), grid, windupDuration, explosionSheets);

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

	CreateExplosion(scene, grid, bombDropCenter, explosionSheets[0], lifetime, { 0,1,2,3 });

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

			// If Breakable Wall is not nullptr - CRUMBLE wall and arm is not spawned
			if (auto* breakableWall = grid.GetBreakableWallAt(currentCol, currentRow))
			{
				if (auto* brWallComp = breakableWall->GetComponent<BreakableWallComponent>())
				{
					brWallComp->Crumble();
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
			CreateExplosion(scene, grid, {gridRectPos.x, gridRectPos.y, 0.f}, sheet, lifetime, animationFrames);
		}
	}
}

void bombGame::spawnUtils::CreateExplosion(ge::Scene& scene, const LevelGrid&,
	const glm::vec3& fixedPosition, ge::SpriteSheet* explosionSheet, float activeTimer, const std::vector<int>& animationFrames)
{
	auto explosion = std::make_unique<ge::GameObject>("GO_ExplosionArm");
	auto* explosionTr{ explosion->GetComponent<ge::Transform>() };
	explosionTr->SetLocalPosition(fixedPosition);
	explosionTr->SetLocalScale(3.4f, 3.4f, 1.f);

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