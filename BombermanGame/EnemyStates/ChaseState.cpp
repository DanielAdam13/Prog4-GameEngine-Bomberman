#include "ChaseState.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/EnemyComponent.h"
#include "WanderState.h"
#include "LevelBuilder.h"
#include <cmath>

bombGame::ChaseState::ChaseState(ge::GameObject* pSourcePtr)
	:EnemyState::EnemyState(pSourcePtr)
{
}

void bombGame::ChaseState::OnEnter()
{
	m_CurrentTarget = FindClosestPlayerInRange();
	// Set it to max from Enter so the new direction is updated INSTANTLY
	m_RefreshClosestTimer = m_RefreshClosestInterval;
}

void bombGame::ChaseState::OnExit()
{
	m_CurrentTarget = nullptr;
	m_DirectionToClosest = {};
}

std::unique_ptr<bombGame::EnemyState> bombGame::ChaseState::OnUpdate(float deltaTime)
{
	// 1. Check for players nearby
	m_RefreshClosestTimer += deltaTime;
	if (m_RefreshClosestTimer >= m_RefreshClosestInterval)
	{
		m_RefreshClosestTimer = 0.f;

		m_CurrentTarget = FindClosestPlayerInRange();

		// 2. Switch to Wander if not Target nearby
		if (!m_CurrentTarget)
		{
			// Transition to Wander
			return std::make_unique<WanderState>(
				GetSourceEnemyComponent()->GetOwner());
		}

		// Movement is handled through ChooseDirectionAtIntersection calls in EnemyComponent
	}

	// 3. Stay in current state
	return nullptr;
}

glm::vec3 bombGame::ChaseState::ChooseDirectionAtIntersection(const GridTile& currentTile)
{
	auto* enemyComp{ GetSourceEnemyComponent() };
	auto* grid{ enemyComp->GetLevelGrid() };
	auto options{ CollectWalkableNeighbors(*grid, currentTile, enemyComp->GetMoveDirection()) };

	// Trapped
	if (options.empty()) 
		return { 0, 0, 0 };

	// Enemy Component will transition to Wander either way so return 0
	if (!m_CurrentTarget)
	{
		return { 0, 0, 0 };
	}

	const auto targetPos{ m_CurrentTarget->GetComponent<ge::Transform>()->GetWorldPosition() };
	const auto targetTile{ grid->GetGridTileAt(targetPos) };

	if (!targetTile) 
		return {0, 0, 0};

	// Pick option that minimizes Manhattan distance to target
	glm::vec3 best = options[0];
	int bestDist{ std::numeric_limits<int>::max() };
	for (auto& dir : options) 
	{
		const int nextCol{ currentTile.col + static_cast<int>(dir.x) };
		const int nextRow{ currentTile.row + static_cast<int>(dir.y) };
		const int newDist{ std::abs(nextCol - targetTile->col) + std::abs(nextRow - targetTile->row) };

		if (newDist < bestDist) 
		{
			bestDist = newDist;
			best = dir;
		}
	}

	return best;
}
