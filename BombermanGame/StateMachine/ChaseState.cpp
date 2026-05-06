#include "ChaseState.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/EnemyComponent.h"
#include <limits>

bombGame::ChaseState::ChaseState(ge::GameObject* pSourcePtr)
	:ge::State::State(pSourcePtr)
{
}

void bombGame::ChaseState::OnUpdate(float deltaTime)
{
	auto* sourceTransform{ GetSource()->GetComponent<ge::Transform>() };
	auto* sourceEnemyController{ GetSource()->GetComponent<EnemyComponent>() };

	glm::vec3 sourcePos{ sourceTransform->GetWorldPosition() };

	glm::vec3 directionToClosestTarget{};
	float closestDistance{ 1000000.f };

	for (auto target : sourceEnemyController->GetTargets())
	{
		const glm::vec3 chaseTargetPos{ target->GetComponent<ge::Transform>()->GetWorldPosition() };
		const glm::vec3 toTarget{ chaseTargetPos - sourcePos };

		const float distSqr{ glm::dot(toTarget, toTarget) };

		if (distSqr < closestDistance)
		{
			closestDistance = distSqr;

			if (distSqr < 0.001f) // avoid normalizing a zero vector
				return;

			const glm::vec3 chaseDirection{ toTarget / std::sqrt(distSqr) };

			directionToClosestTarget = chaseDirection;
		}
	}
	
	
	sourceEnemyController->SetMoveDirection(directionToClosestTarget);

	sourcePos += directionToClosestTarget * sourceEnemyController->GetSpeed() * deltaTime;

	sourceTransform->SetLocalPosition({ sourcePos.x, sourcePos.y, 0.f });
}
