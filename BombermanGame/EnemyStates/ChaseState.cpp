#include "ChaseState.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/EnemyComponent.h"
#include <limits>

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

void bombGame::ChaseState::OnUpdate(float deltaTime)
{
	m_RefreshClosestTimer += deltaTime;
	if (m_RefreshClosestTimer >= m_RefreshClosestInterval)
	{
		m_RefreshClosestTimer = 0.f;

		ge::GameObject* closest{ FindClosestPlayerInRange() };
		if (!closest)
		{
			// Transition to Wander
			GetSourceEnemyComponent()->TransitionToWander();
			return;
		}

		if (closest != m_CurrentTarget)
		{
			m_CurrentTarget = closest;
		}

		m_DirectionToClosest = glm::normalize(m_CurrentTarget->GetComponent<ge::Transform>()->GetWorldPosition() -
			GetSourceTransform()->GetWorldPosition());
	}
	
	GetSourceEnemyComponent()->SetMoveDirection(m_DirectionToClosest);

	const glm::vec3 newPos{ GetSourceTransform()->GetWorldPosition() + 
		m_DirectionToClosest * GetSourceEnemyComponent()->GetSpeed() * deltaTime};
	GetSourceTransform()->SetLocalPosition({newPos.x, newPos.y, 0.f});
}

void bombGame::ChaseState::OnExit()
{
	m_CurrentTarget = nullptr;
	m_DirectionToClosest = {};
}
