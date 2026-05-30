#include "ChaseState.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/EnemyComponent.h"
#include "WanderState.h"

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

		const auto targetPos{ m_CurrentTarget->GetComponent<ge::Transform>()->GetWorldPosition() };
		const auto myPos{ GetSourceTransform()->GetWorldPosition() };
		const auto diff{ targetPos - myPos };

		if (glm::length(diff) > 0.0001f)
		{
			m_DirectionToClosest = glm::normalize(diff);
		}
		
		// 3. Update direction to closest target
		m_DirectionToClosest = glm::normalize(m_CurrentTarget->GetComponent<ge::Transform>()->GetWorldPosition() -
			GetSourceTransform()->GetWorldPosition());
	}

	GetSourceEnemyComponent()->SetMoveDirection(m_DirectionToClosest);

	// 4. Stay in current state
	return nullptr;
}
