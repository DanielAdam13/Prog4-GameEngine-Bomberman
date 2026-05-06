#include "EnemyComponent.h"

#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/Transform.h"

bombGame::EnemyComponent::EnemyComponent(ge::GameObject* owner, float speed, float detectionRadius)
	:Component::Component(owner),
	m_OwnerTransformRef{ GetOwner()->GetComponent<ge::Transform>() },
	m_Speed{speed},
	m_DetectionRadius{ detectionRadius }
{
}

void bombGame::EnemyComponent::AddTarget(ge::GameObject* newTarget) noexcept
{
	if (!newTarget)
		return;

	auto* targetTr{ newTarget->GetComponent<ge::Transform>() };

	m_Targets.push_back(newTarget);
	m_TargetTransforms.push_back(targetTr);
}

bool bombGame::EnemyComponent::IsAlive() const noexcept
{
	auto* healthComp{ GetOwner()->GetComponent<ge::HealthComponent>() };
	return healthComp && !healthComp->IsDead();
}
