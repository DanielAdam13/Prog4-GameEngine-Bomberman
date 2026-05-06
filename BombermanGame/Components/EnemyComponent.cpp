#include "EnemyComponent.h"

#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/Transform.h"

bombGame::EnemyComponent::EnemyComponent(ge::GameObject* owner, float speed, float detectionRadius)
	:Component::Component(owner),
	m_Targets{},
	m_OwnerTransformRef{GetOwner()->GetComponent<ge::Transform>()},
	m_Speed{speed},
	m_CurrentMoveDirection{},
	m_DetectionRadius{ detectionRadius }
{
}

//void bombGame::EnemyComponent::UpdateComponent(float deltaTime)
//{
//	if (IsAlive())
//	{
//		const glm::vec3 movementVector{ m_CurrentMoveDirection * m_Speed * deltaTime };
//		m_OwnerTransformRef->SetLocalPosition(m_OwnerTransformRef->GetWorldPosition() + movementVector);
//	}
//	
//}

//void bombGame::EnemyComponent::RenderComponent() const
//{
//}

void bombGame::EnemyComponent::AddTarget(ge::GameObject* newTarget) noexcept
{
	if (!newTarget)
		return;

	m_Targets.push_back(newTarget);

}

bool bombGame::EnemyComponent::IsAlive() const noexcept
{
	auto* healthComp{ GetOwner()->GetComponent<ge::HealthComponent>() };
	return healthComp && !healthComp->IsDead();
}
