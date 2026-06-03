#include "BaseEnemyState.h"

#include "GameObject.h"
#include "Components/EnemyComponent.h"
#include "Components/PlayerComponent.h"
#include "LevelGrid.h"

#include <cassert>
#include <limits>
#include <glm/glm.hpp>

bombGame::EnemyState::EnemyState(ge::GameObject* pTargetPtr)
	:m_CachedEnemyComponent{ pTargetPtr->GetComponent<EnemyComponent>() },
	m_CachedSourceTransform{ pTargetPtr->GetComponent<ge::Transform>() }
{
	assert(m_CachedEnemyComponent && "An Enemy State requires target having an Enemy Component!");
}

ge::GameObject* bombGame::EnemyState::FindClosestPlayerInRange() const
{
	const float range{ m_CachedEnemyComponent->GetDetectionRadius() };
	const float rangeSqr{ range * range };
	const auto enemyPos{ m_CachedSourceTransform->GetWorldPosition() };

	ge::GameObject* closestTarget{ nullptr };
	float closestDistSqr{ std::numeric_limits<float>::max() };

	for (auto* target : m_CachedEnemyComponent->GetTargets())
	{
		if (!target)
			return nullptr;

		// Don't count dead players
		auto targetPc{ target->GetComponent<PlayerComponent>() };
		if (!targetPc || !targetPc->IsAlive())
			continue;

		const glm::vec3 targetPos{ target->GetComponent<ge::Transform>()->GetWorldPosition() };
		const glm::vec2 diff{ targetPos - enemyPos };

		const float distanceSqr{ glm::dot(diff, diff) };

		if (distanceSqr <= rangeSqr && distanceSqr < closestDistSqr)
		{
			closestDistSqr = distanceSqr;
			closestTarget = target;
		}
	}

	return closestTarget;
}

bombGame::EnemyComponent* bombGame::EnemyState::GetSourceEnemyComponent() const noexcept
{
	return m_CachedEnemyComponent;
}

ge::Transform* bombGame::EnemyState::GetSourceTransform() const noexcept
{
	return m_CachedSourceTransform;
}

std::vector<glm::vec3> bombGame::EnemyState::CollectWalkableNeighbors(const LevelGrid& grid, 
	const GridTile& currentTile, const glm::vec3& currentDir) const
{
	std::vector<glm::vec3> options;
	
	// Look for VALID directions.
	// A valid direction would mean - neighbour is a valid tile, tile is not Wall, tile is not BREAKABLE wall, direction is not the same
	for (auto [dx, dy] : m_PossibleDirections)
	{
		auto neighbour{ grid.GetGridTileByCoord(currentTile.col + dx, currentTile.row + dy) };
		if (!neighbour)
			continue;
		if (neighbour->gridTileType == levelLoader::TileType::Wall)
			continue;
		if (grid.GetBreakableWallAt(currentTile.col + dx, currentTile.row + dy))
			continue;

		const glm::vec3 dir{ dx, dy, 0 };

		if (dir == -currentDir)
			continue;

		options.push_back(dir);
	}

	return options;
}
