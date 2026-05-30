#include "BaseEnemyState.h"

#include "GameObject.h"
#include "Components/EnemyComponent.h"
#include "Components/PlayerComponent.h"

#include <cassert>
#include <limits>
#include <glm/glm.hpp>

bombGame::EnemyState::EnemyState(ge::GameObject* pTargetPtr)
	:m_pEnemyComponent{ pTargetPtr->GetComponent<EnemyComponent>() },
	m_pSourceTransform{ pTargetPtr->GetComponent<ge::Transform>() }
{
	assert(m_pEnemyComponent && "An Enemy State requires target having an Enemy Component!");
}

ge::GameObject* bombGame::EnemyState::FindClosestPlayerInRange() const
{
	const float range{ m_pEnemyComponent->GetDetectionRadius() };
	const float rangeSqr{ range * range };
	const auto enemyPos{ m_pSourceTransform->GetWorldPosition() };

	ge::GameObject* closestTarget{ nullptr };
	float closestDistSqr{ std::numeric_limits<float>::max() };

	for (auto* target : m_pEnemyComponent->GetTargets())
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
	return m_pEnemyComponent;
}

ge::Transform* bombGame::EnemyState::GetSourceTransform() const noexcept
{
	return m_pSourceTransform;
}
