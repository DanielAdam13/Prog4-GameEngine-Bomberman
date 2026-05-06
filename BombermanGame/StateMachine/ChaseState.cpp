#include "ChaseState.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/EnemyComponent.h"
#include <limits>

bombGame::ChaseState::ChaseState(ge::GameObject* pSourcePtr)
	:ge::State::State(pSourcePtr),
	m_pSourceTransform{ pSourcePtr->GetComponent<ge::Transform>() },
	m_pEnemyController{ pSourcePtr->GetComponent<EnemyComponent>() }
{
}

void bombGame::ChaseState::OnUpdate(float deltaTime)
{
	const glm::vec3 sourcePos{ m_pSourceTransform->GetWorldPosition() };

	glm::vec3 directionToClosestTarget{};
	float closestDistSqr{ std::numeric_limits<float>::max() };

	for (auto* targetTransform : m_pEnemyController->GetTargetTransforms())
	{
		const glm::vec3 toTarget{ targetTransform->GetWorldPosition() - sourcePos };
		const float distSqr{ glm::dot(toTarget, toTarget) };

		if (distSqr < closestDistSqr && distSqr > 0.001f) // avoid normalizing a zero vector
		{
			closestDistSqr = distSqr;
			directionToClosestTarget = toTarget / std::sqrt(distSqr);
		}
	}
	
	if (closestDistSqr == std::numeric_limits<float>::max())
		return; // guard just in case
	
	m_pEnemyController->SetMoveDirection(directionToClosestTarget);

	const glm::vec3 newPos{ sourcePos + directionToClosestTarget * m_pEnemyController->GetSpeed() * deltaTime };
	m_pSourceTransform->SetLocalPosition({ newPos.x, newPos.y, 0.f });
}
