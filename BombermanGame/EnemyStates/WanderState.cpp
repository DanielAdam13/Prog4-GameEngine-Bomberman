#include "WanderState.h"

#include "GameObject.h"
#include "Components/EnemyComponent.h"
#include "Components/Transform.h"
#include "ChaseState.h"

#include <random>

bombGame::WanderState::WanderState(ge::GameObject* pSourcePtr)
	:EnemyState::EnemyState(pSourcePtr)
{
}

void bombGame::WanderState::OnEnter()
{
	m_WanderTimer = 0.f;
	m_PlayerCheckTimer = 0.f;
}

std::unique_ptr<bombGame::EnemyState> bombGame::WanderState::OnUpdate(float deltaTime)
{
	// 1. Is there a close target check
	m_PlayerCheckTimer += deltaTime;
	if (m_PlayerCheckTimer >= m_PlayerCheckInterval)
	{
		m_PlayerCheckTimer = 0.f;

		// IF CLOSEST PLAYER TARGET is not nullptr, if it exists
		if (FindClosestPlayerInRange()) 
		{
			// Transition to chase
			return std::make_unique<ChaseState>(GetSourceEnemyComponent()->GetOwner());
		}
	}

	// Movement is handled through ChooseDirectionAtIntersection calls in EnemyComponent

	// 2. Stay in current state
	return nullptr;
}

glm::vec3 bombGame::WanderState::ChooseDirectionAtIntersection(const GridTile& currentTile)
{
	auto* grid{ GetSourceEnemyComponent()->GetLevelGrid() };
	auto options{ CollectWalkableNeighbors(*grid, currentTile, GetSourceEnemyComponent()->GetMoveDirection()) };

	// Trapped
	if (options.empty())
		return { 0, 0, 0 };

	static std::mt19937 gen{ std::random_device{}() };
	std::uniform_int_distribution<size_t> pick(0, options.size() - 1);
	return options[pick(gen)];
}
